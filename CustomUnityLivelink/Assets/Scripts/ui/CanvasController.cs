using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class CanvasController : MonoBehaviour
{
    public GameObject panel;
    public Button btn_setting;
    public Toggle toggle_skeleton_display;
    public Dropdown dropdown_max_person_num;
    public List<Dropdown> dropdown_avatar_list = new List<Dropdown>(new Dropdown[8]);
    public List<GameObject> person_list;
    public List<GameObject> edge_device_list;
    public Text fusion_fps;
    public List<Text> edge_device_sn_list = new List<Text>(new Text[10]);
    public List<Text> edge_device_fps_list = new List<Text>(new Text[10]);

    private Color[] colors = new Color[]{
        new Color( 232.0f / 255.0f, 176.0f / 255.0f,59.0f / 255.0f),
        new Color(175.0f / 255.0f, 208.0f / 255.0f,25.0f / 255.0f),
        new Color(102.0f / 255.0f / 255.0f, 205.0f / 255.0f,105.0f / 255.0f),
        new Color(185.0f / 255.0f, 0.0f / 255.0f,255.0f / 255.0f),
        new Color(99.0f / 255.0f, 107.0f / 255.0f,252.0f / 255.0f),
        new Color(252.0f / 255.0f, 225.0f / 255.0f, 8.0f / 255.0f),
        new Color(167.0f / 255.0f, 130.0f / 255.0f, 141.0f / 255.0f),
        new Color(194.0f / 255.0f, 72.0f / 255.0f, 113.0f / 255.0f)
    };

    private int limit_person_num = 8;
    private int limit_edge_device_num = 10;

    public bool is_display_skeleton = true;
    public int max_person_num = 4;
    public List<int> curr_avatar_list = new List<int>(new int[8]);

    void Start() {
        panel.SetActive(false);
        btn_setting.onClick.AddListener(PopupSetting);
        toggle_skeleton_display.onValueChanged.AddListener(SetSkeletonDisplay);

        for (int i = 0; i < limit_person_num; i++) {
            dropdown_avatar_list[i] = person_list[i].transform.Find("DropdownAvatar").GetComponent<Dropdown>();
            person_list[i].transform.Find("ImageColor").GetComponent<Image>().color = colors[i];
            curr_avatar_list[i] = dropdown_avatar_list[i].value;
        }

        for (int i = 0; i < limit_edge_device_num; i++) {
            edge_device_sn_list[i] = edge_device_list[i].transform.Find("TextEdgeDeviceSN").GetComponent<Text>();
            edge_device_fps_list[i] = edge_device_list[i].transform.Find("TextEdgeDeviceFPS").GetComponent<Text>();
            edge_device_sn_list[i].color = colors[i % limit_person_num];
            edge_device_fps_list[i].color = colors[i % limit_person_num];
        }
    }

    // Update is called once per frame
    void Update() {
        max_person_num = dropdown_max_person_num.value + 1;
        for (int i = 0; i < max_person_num; i++) {
            dropdown_avatar_list[i].interactable = true;
            // curr_avatar_list[i] = dropdown_avatar_list[i].value;
        }

        for (int i = max_person_num; i < 8; i++) {
            dropdown_avatar_list[i].interactable = false;
        }
    }

    void Initialize() {

    }

    void PopupSetting() {
        Debug.Log("PopupSetting");
        panel.SetActive(true);
    }

    void SetPersonColor(int id, Color color) {
        if(id < person_list.Count) {
            person_list[id].transform.Find("ImageColor").GetComponent<Image>().color = color;
        } else {
            Debug.Log("id is out of range");
        }
    }

    void SetSkeletonDisplay(bool isOn) {
        Debug.Log("SetSkeletonDisplay : " + isOn.ToString());
        is_display_skeleton = isOn;
    }

    public bool IsAvatarChanged(int person_id) {
        int id = person_id % max_person_num;
        bool ret = false;
        if (curr_avatar_list[id] != dropdown_avatar_list[id].value) {
            ret = true;
            curr_avatar_list[id] = dropdown_avatar_list[id].value;
        }
        return ret;
    }

    public int GetAssignedID(int person_id) {
        int id = person_id % max_person_num;
        int ret = curr_avatar_list[id];
        return ret;
    }

    public void UpdateFusionPerformance(string fps) {
        fusion_fps.text = fps;
    }

    public void UpdatePerformance(int id, string serial_number, string fps) {
        if (id < edge_device_list.Count) {
            edge_device_sn_list[id].text = serial_number;
            edge_device_fps_list[id].text = fps;
        } else {
            Debug.Log("id is out of range");
        }
    }
}
