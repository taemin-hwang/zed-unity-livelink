using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class MoveCamera : MonoBehaviour
{
    // public GameObject target;
    private Camera mainCamera;
    private float xRotateMove, yRotateMove;
    public float rotateSpeed = 500.0f;
    public float zoomSpeed = 20.0f;

    void Start(){
        mainCamera = GetComponent<Camera>();
        Vector3 target_pos = new Vector3(0.0f, 0.0f, 0.0f);
        transform.LookAt(target_pos);
    }

    void Update(){
        Zoom();
        Rotate();
    }

    public bool IsPointerOverUIObject(Vector2 touchPos)
    {
        // return false;
        PointerEventData eventDataCurrentPosition = new PointerEventData(EventSystem.current);
        eventDataCurrentPosition.position = touchPos;
        List<RaycastResult> results = new List<RaycastResult>();
        EventSystem.current.RaycastAll(eventDataCurrentPosition, results);
        return results.Count > 0;
    }

    private void Zoom(){
        float distance = Input.GetAxis("Mouse ScrollWheel") * -1 * zoomSpeed;
        if(distance != 0) {
            mainCamera.fieldOfView += distance;
        }
    }

    private void Rotate(){
        if (Input.GetMouseButton(0)){
            if(!IsPointerOverUIObject(Input.mousePosition)) {
                xRotateMove = Input.GetAxis("Mouse X") * Time.deltaTime * rotateSpeed;
                yRotateMove = Input.GetAxis("Mouse Y") * Time.deltaTime * rotateSpeed;
                // Vector3 target_pos = target.transform.position;
                Vector3 target_pos = new Vector3(0.0f, 0.0f, 0.0f);
                transform.RotateAround(target_pos, Vector3.up, xRotateMove);
                transform.RotateAround(target_pos, Vector3.right, yRotateMove);
                transform.LookAt(target_pos);
            }
        }
    }
}
