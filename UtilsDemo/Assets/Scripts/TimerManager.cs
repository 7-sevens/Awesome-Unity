using System.Collections.Generic;
using UnityEngine;
using System;
using DG.Tweening;

public class TimerManager : Singletion<TimerManager>
{
    private class TimerData
    {
        public Sequence Sequence;
        public int timerID;       //定时器ID
    }

    private int m_timerID = 1;
    private Dictionary<int, TimerData> m_timers = new Dictionary<int, TimerData>();


    //获取从1970年1月1日到现在的毫秒数
    public long getTime()
    {
        TimeSpan ts = DateTime.Now - System.DateTime.Parse("1970-1-1");
        return Convert.ToInt64(ts.TotalMilliseconds);
    }

    public double getTimeAsNumber()
    {
        TimeSpan ts = DateTime.Now - System.DateTime.Parse("1970-1-1");
        return Math.Floor(Convert.ToDouble(ts.TotalMilliseconds));
    }

    public int setTimer(float interval, int repeat, Action/*<object[]>*/ listener/*, object[] objs = null*/)
    {
        if (repeat == 0) { return -1; }

        TimerData td = new TimerData();
        td.Sequence = DOTween.Sequence();
        td.Sequence.SetLoops(repeat);
        td.Sequence.AppendInterval(interval);
        td.Sequence.AppendCallback(delegate ()
        {
            listener(/*objs*/);
        });
        td.timerID = m_timerID;
        m_timerID++;
        m_timers[td.timerID] = td;
        return td.timerID;
    }
    public void killTimer(int timerID)
    {
        if (m_timers.ContainsKey(timerID))
        {
            m_timers[timerID].Sequence.Kill(false);
            m_timers.Remove(timerID);
        }
    }
}
