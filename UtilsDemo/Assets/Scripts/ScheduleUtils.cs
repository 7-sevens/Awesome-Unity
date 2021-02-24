using DG.Tweening;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

public class ScheduleUtils : Singletion<ScheduleUtils>
{
    private Dictionary<string, int> _dicTimers = new Dictionary<string, int>();

    public void init()
    {

    }


    //schedule(callback: Function, interval?: number, repeat?: number, delay?: number)
    // repeat : -1 - 无限循环
    // 循环定时器
    public void schedule(Action callback, float interval, int repeat, string key)
    {
        if (_dicTimers.ContainsKey(key))
            return;

        int tID = TimerManager.getInstance().setTimer(interval * 1000, repeat, callback);
        _dicTimers[key] = tID;
    }


    // scheduleOnce(callback: Function, delay?: number) : void;		
    public void scheduleOnce(Action callback, float interval)
    {
        Sequence seq = DOTween.Sequence();
        seq.AppendInterval(interval);
        seq.AppendCallback(delegate () { callback(); });
    }

    // unschedule(callback_fn: Function): void;	
    public void unschedule(string key)
    {
        if (_dicTimers.ContainsKey(key))
        {
            TimerManager.getInstance().killTimer(_dicTimers[key]);
        }
    }

    //unscheduleAllCallbacks(): void;	
}
