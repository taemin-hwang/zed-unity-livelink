//======= Copyright (c) Stereolabs Corporation, All rights reserved. ===============

using UnityEngine;
using System.Collections.Generic;

/// <summary>
///
/// </summary>
[DisallowMultipleComponent]
public class MyZEDBodyTrackingManager : MonoBehaviour
{
    public enum BODY_MODE
    {
        FULL_BODY = 0,
        UPPER_BODY = 1
    }

    #region vars
    /// <summary>
    /// Vizualisation mode. Use a 3D model or only display the skeleton
    /// </summary>
    [Header("Vizualisation Mode")]
    /// <summary>
    /// Display 3D avatar. If set to false, only display bones and joint
    /// </summary>
    [Tooltip("Display 3D avatar. If set to false, only display bones and joint")]
    public bool enableAvatar = true;

    /// <summary>
    /// Maximum number of detection displayed in the scene.
    /// </summary>
    [Tooltip("Maximum number of detections spawnable in the scene")]
    public int maximumNumberOfDetections = 75;

    /// <summary>
    /// Avatar game objects
    /// </summary>
    [Tooltip("3D Rigged model.")]
    public GameObject[] avatars;

    [Space(5)]

    [Tooltip("Display bones and joints along 3D avatar")]
    [SerializeField]
    private bool enableSDKSkeleton = false;
    public static bool EnableSDKSkeleton = false;
    [SerializeField]
    private Vector3 offsetSDKSkeleton = new Vector3 (1f,0f,0f);
    public static Vector3 OffsetSDKSkeleton = new Vector3(1f, 0f, 0f);
    public Material skeletonBaseMaterial;

    [Space(5)]
    [Header("Other settings")]
    [Tooltip("Which body mode to use: \nFULL_BODY uses the root position to move the avatar and the local rotations to animate all the limbs." +
        "\nUPPER_BODY uses the navigation system and animates the legs to match the movement in space, and animates the body from the hips and above with the local rotations from the ZED SDK.")]
    public BODY_MODE bodyMode = BODY_MODE.FULL_BODY;
    [Tooltip("Mirror the animation.")]
    public bool mirrorMode = false;


    [Header("MOVEMENT SMOOTHING SETTINGS")]
    [Tooltip("Expected frequency of reception of new Body Tracking data, in FPS")]
    private float bodyTrackingFrequency = 30f;
    public static float BodyTrackingFrequency = 30f;
    [Tooltip("Factor for the interpolation duration. " +
        "\n0=>instant movement, no lerp; 1=>Rotation of the SDK should be done between two frames. More=>Interpolation will be longer, latency grow but movements will be smoother.")]
    private float smoothingFactor = 5f;
    public static float SmoothingFactor = 5f;

    public Dictionary<int,MySkeletonHandler> avatarControlList;
    public MyZEDStreamingClient zedStreamingClient;
    public CanvasController canvasController;

#endregion

    /// <summary>
    /// Start this instance.
    /// </summary>
    private void Start()
    {
        QualitySettings.vSyncCount = 1; // Activate vsync

        canvasController = FindObjectOfType<CanvasController>();
        avatarControlList = new Dictionary<int,MySkeletonHandler> ();

        if (!zedStreamingClient)
        {
            zedStreamingClient = FindObjectOfType<MyZEDStreamingClient>();
        }

        zedStreamingClient.OnNewDetection += UpdateSkeletonData;
        zedStreamingClient.OnPerformance += canvasController.UpdatePerformance;
        zedStreamingClient.OnFusionPerformance += canvasController.UpdateFusionPerformance;
    }

    private void OnDestroy()
    {
        if (zedStreamingClient)
        {
            zedStreamingClient.OnNewDetection -= UpdateSkeletonData;
            zedStreamingClient.OnPerformance -= canvasController.UpdatePerformance;
            zedStreamingClient.OnFusionPerformance -= canvasController.UpdateFusionPerformance;
        }
    }

    /// <summary>
    /// Updates the skeleton data from ZEDCamera call and send it to Skeleton Handler script.
    /// </summary>
    private void UpdateSkeletonData(sl.Bodies bodies)
    {
        List<int> remainingKeyList = new List<int>(avatarControlList.Keys);
        List<sl.BodyData> newBodies = new List<sl.BodyData>(bodies.body_list);

        foreach (sl.BodyData bodyData in newBodies)
        {
            int person_id = bodyData.id;

            if (bodyData.tracking_state == sl.OBJECT_TRACK_STATE.OK)
            {
                //Avatar controller already exist --> update position
                if (canvasController.IsAvatarChanged(person_id)) {
                    if (avatarControlList.Count < maximumNumberOfDetections) {
                        // Destroy old avatar
                        MySkeletonHandler handler = avatarControlList[person_id];
                        handler.Destroy();
                        avatarControlList.Remove(person_id);

                        // Create new avatar
                        handler = ScriptableObject.CreateInstance<MySkeletonHandler>();
                        Vector3 spawnPosition = bodyData.position;
                        int assigned_id = canvasController.GetAssignedID(person_id);

                        handler.Create(avatars[assigned_id], bodies.body_format);
                        handler.InitSkeleton(person_id, new Material(skeletonBaseMaterial));

                        avatarControlList.Add(person_id, handler);
                        UpdateAvatarControl(handler, bodyData);
                    }
                } else if (avatarControlList.ContainsKey(person_id)) {
                    MySkeletonHandler handler = avatarControlList[person_id];
                    UpdateAvatarControl(handler, bodyData);

                    // remove keys from list
                    remainingKeyList.Remove(person_id);
                } else {
                    if (avatarControlList.Count < maximumNumberOfDetections) {
                        MySkeletonHandler handler = ScriptableObject.CreateInstance<MySkeletonHandler>();
                        Vector3 spawnPosition = bodyData.position;
                        // handler.Create(avatars[person_id % avatars.Length], bodies.body_format);
                        // handler.InitSkeleton(person_id, new Material(skeletonBaseMaterial));
                        int assigned_id = canvasController.GetAssignedID(person_id);

                        handler.Create(avatars[assigned_id], bodies.body_format);
                        handler.InitSkeleton(person_id, new Material(skeletonBaseMaterial));

                        avatarControlList.Add(person_id, handler);
                        UpdateAvatarControl(handler, bodyData);
                    }
                }
            }
        }

        foreach (int index in remainingKeyList)
        {
            MySkeletonHandler handler = avatarControlList[index];
            handler.Destroy();
            avatarControlList.Remove(index);
        }
    }

    public void Update()
    {
        // EnableSDKSkeleton = enableSDKSkeleton;
        EnableSDKSkeleton = canvasController.is_display_skeleton;
        OffsetSDKSkeleton = offsetSDKSkeleton;
        BodyTrackingFrequency = bodyTrackingFrequency;
        SmoothingFactor = smoothingFactor;

        if (Input.GetKeyDown(KeyCode.Space))
        {
            enableAvatar = !enableAvatar;
            if (enableAvatar)
            {
                Debug.Log("<b><color=green> Switch to Avatar mode</color></b>");

            }
            else
                Debug.Log("<b><color=green> Switch to Skeleton mode</color></b>");
        }

        // Adjust the 3D avatar to the bones rotations from the SDK each frame.
        // These rotations are stored, and updated each time data is received from Fusion.
        if (enableAvatar)
        {
            if (bodyMode == BODY_MODE.FULL_BODY)
            {
                foreach (var skelet in avatarControlList)
                {
                    skelet.Value.Move();
                }
            }
            else if (bodyMode == BODY_MODE.UPPER_BODY)
            {
                foreach (var skelet in avatarControlList)
                {
                    skelet.Value.UpdateNavigationDataUpperBody();
                }
            }
        }
    }

    private void LateUpdate()
    {
        if (bodyMode == BODY_MODE.UPPER_BODY)
        {
            foreach (var skelet in avatarControlList)
            {
                skelet.Value.UpdateRotationsUpperBody();
            }
        }
    }

    /// <summary>
    /// Function to update avatar control with data from ZED SDK.
    /// </summary>
    /// <param name="handler">Handler.</param>
    /// <param name="data">Body tracking data.</param>
    private void UpdateAvatarControl(MySkeletonHandler handler, sl.BodyData data)
    {
        if (data.local_orientation_per_joint.Length > 0 && data.keypoint.Length > 0 && data.keypoint_confidence.Length > 0)
        {
            handler.SetConfidences(data.keypoint_confidence);
            handler.SetControlWithJointPosition(
                data.keypoint,
                data.local_orientation_per_joint, data.global_root_orientation,
                enableAvatar, mirrorMode);

            // Also update velocity for legs animation
            if (bodyMode == BODY_MODE.UPPER_BODY)
            {
                handler.rootVelocity = data.velocity;
            }
        }
    }
}
