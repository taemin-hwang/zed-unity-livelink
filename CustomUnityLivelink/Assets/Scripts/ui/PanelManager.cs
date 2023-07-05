using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PanelManager : MonoBehaviour
{
    public GameObject canvas;
    public Button btnFinish;
    public CanvasController canvasController;

    // Start is called before the first frame update
    void Start()
    {
        btnFinish.onClick.AddListener(PopoutSetting);
        canvasController = canvas.GetComponent<CanvasController>();
    }

    // Update is called once per frame
    void Update()
    {

    }

    void PopoutSetting() {
        gameObject.SetActive(false);
        // sliderPlaneWidth.onValueChanged.AddListener(delegate {OnWidthChanged();});
        // sliderPlaneHeight.onValueChanged.AddListener(delegate {OnHeightChanged();});
        // sliderCameraRotation.onValueChanged.AddListener(delegate {OnRotationChanged();});
        // sliderSkeletonSize.onValueChanged.AddListener(delegate {OnSkeletonSizeChanged();});
        // sliderHandLength.onValueChanged.AddListener(delegate {OnHandLengthChanged();});
        // toggleDisplaySkeleton.onValueChanged.AddListener(OnDisplaySkeleton);
        // PlayerPrefs.SetFloat("Width", canvasController.sliderPlaneWidth.value);
        // PlayerPrefs.SetFloat("Height", canvasController.sliderPlaneHeight.value);
        // PlayerPrefs.SetFloat("Rotation", canvasController.sliderCameraRotation.value);
        // PlayerPrefs.SetFloat("Size", canvasController.sliderSkeletonSize.value);
        // PlayerPrefs.SetFloat("HandLength", canvasController.sliderHandLength.value);
        // int ison = 0;
        // if (canvasController.toggleDisplaySkeleton.isOn == false) {
        //     ison = 0;
        // } else {
        //     ison = 1;
        // }
        // PlayerPrefs.SetInt("IsOn", ison);
    }
}
