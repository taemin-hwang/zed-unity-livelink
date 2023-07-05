using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class DropDownController : MonoBehaviour
{
    string DROPDOWN_KEY = "DROPDOWN_KEY";

    int currentOption;
    Dropdown options;

    List<string> optionList = new List<string>();

    void Awake()
    {
        GameObject parents = transform.parent.gameObject;
        string parentsName = parents.name;
        DROPDOWN_KEY = parentsName + DROPDOWN_KEY;

        if (PlayerPrefs.HasKey(DROPDOWN_KEY) == false) currentOption = 0;
        else currentOption = PlayerPrefs.GetInt(DROPDOWN_KEY);
    }

    void Start()
    {
        options = this.GetComponent<Dropdown>();

        options.ClearOptions();

        optionList.Add("Avatar 1");
        optionList.Add("Avatar 2");
        optionList.Add("Avatar 3");
        optionList.Add("Avatar 4");
        optionList.Add("Avatar 5");
        optionList.Add("Avatar 6");
        optionList.Add("Avatar 7");
        optionList.Add("Avatar 8");

        options.AddOptions(optionList);

        options.value = currentOption;

        options.onValueChanged.AddListener(delegate { setDropDown(options.value); });
        setDropDown(currentOption);
    }

    void setDropDown(int option)
    {
        PlayerPrefs.SetInt(DROPDOWN_KEY, option);

        Debug.Log("current option : " + option);
    }
}