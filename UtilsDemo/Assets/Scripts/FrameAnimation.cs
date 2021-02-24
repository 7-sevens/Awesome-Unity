using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class FrameAnimation : MonoBehaviour
{

    public Sprite[] spriteArray;
    Image img;
    public float durationTime;
    public float delayTime;
    public bool showOnStartFlag = true;
    public bool isLoop;
    public bool isStop;
    public bool isSetNativeSize = false;

    int count = 0;
    float currentTime;
    bool breakFlg = false;
    bool beginFlag = false;

    void Awake()
    {
        img = this.GetComponent<Image>();
        img.enabled = showOnStartFlag;
    }

    // Use this for initialization
    void Start()
    {
        currentTime = Time.time;
        img.sprite = spriteArray[count];
    }

    void OnDisable()
    {
        count = 0;
        breakFlg = false;
    }

    // Update is called once per frame
    void Update()
    {
        float intervalTime = Time.time - currentTime;
        if (intervalTime >= delayTime && !beginFlag)
        {
            beginFlag = true;
            img.enabled = true;
        }

        if (!beginFlag)
        {
            return;
        }

        if (isStop)
        {
            return;
        }

        if (breakFlg)
        {
            return;
        }

        float fTime = Time.time - currentTime;
        if (fTime >= durationTime)
        {
            count++;
            if (count >= spriteArray.Length)
            {
                if (isLoop)
                {
                    breakFlg = false;
                    count = 0;
                }
                else
                {
                    breakFlg = true;
                    return;
                }
            }

            img.sprite = spriteArray[count];
            if (isSetNativeSize)
            {
                img.SetNativeSize();
            }
            currentTime = Time.time;
        }
    }

}
