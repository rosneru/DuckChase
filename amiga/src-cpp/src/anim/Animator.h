#ifndef ANIMATOR_H
#define ANIMATOR_H


template <class S, class T>
class Animator 
{
  public:
    Animator(S& shape, T* pAnimSeq)
      : m_Shape(shape),
        m_pAnimSeq(pAnimSeq),
        m_CurrentFrameId(0),
        m_bAnimateBackward(false)

    {
      FirstFrame();
    }


    ~Animator()
    {

    }


    void FirstFrame()
    {
      if((m_pAnimSeq == NULL) || (m_pAnimSeq->NumFrames() == 0))
      {
        return;
      }

      // Select the first image
      m_CurrentFrameId = 0;

      m_Shape.SetImage((*m_pAnimSeq)[m_CurrentFrameId]);

      // WORD* pFirstImage = m_pAnimSeq->NumFrames();
    }

    void NextFrame(size_t animStartImageIdx = 0)
    {
      if((m_pAnimSeq == NULL) || (m_pAnimSeq->NumFrames() == 0))
      {
        return;
      }

      // Setting m_CurrentFrameId to the next anim frame
      setNextAnimFrameId(animStartImageIdx);

      m_Shape.SetImage((*m_pAnimSeq)[m_CurrentFrameId]);
    }


    void SetAnimSeq(T* pAnimSeq)
    {
      m_pAnimSeq = pAnimSeq;
    }

    void SetAnimateBackward(bool bEnabled)
    {
      m_bAnimateBackward = bEnabled;
    }

    // const T operator[](size_t index) const
    // {
    //   return *(m_pData + index);
    // }


private:
  S& m_Shape;
  T* m_pAnimSeq;
  size_t m_CurrentFrameId;
  bool m_bAnimateBackward;

  void setNextAnimFrameId(size_t animStartImageIdx)
  {
    if(m_pAnimSeq == NULL)
    {
      return;
    }

    if(m_bAnimateBackward)
    {
      if(m_CurrentFrameId == 0)
      {
        // Select the last image
        m_CurrentFrameId = m_pAnimSeq->NumFrames() - 1;
      }
      else
      {
        // Select the previous image
        m_CurrentFrameId--;
        if(m_CurrentFrameId == animStartImageIdx)
        {
          m_CurrentFrameId = m_pAnimSeq->NumFrames() - 1;
        }
      }
    }
    else
    {
      // Select the next image
      m_CurrentFrameId++;
      if(m_CurrentFrameId >= m_pAnimSeq->NumFrames())
      {
        m_CurrentFrameId = animStartImageIdx;
      }
    }
  }


};

#endif

