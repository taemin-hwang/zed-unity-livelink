using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NumberRotator : MonoBehaviour
{
    Camera mainCamera;
    // Start is called before the first frame update
    void Start()
    {
        mainCamera = Camera.main;
    }

    // Update is called once per frame
    void Update()
    {
        mainCamera = Camera.main;

        if (mainCamera != null)
        {
            // Calculate the target rotation by facing the main camera
            Quaternion targetRotation = Quaternion.LookRotation(mainCamera.transform.position - transform.position, Vector3.up);

            // Apply the rotation
            transform.rotation = targetRotation * Quaternion.Euler(-90f, 0f, 0f);
        }
    }
}
