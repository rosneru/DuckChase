#include <stdlib.h>

#include "EntityCollection.h"


EntityCollection::EntityCollection()
  : m_Capacity(MIN_CAPACITY),
    m_Size(0),
    m_pData((EntityBase**)malloc(m_Capacity * sizeof(EntityBase*)))
{ 
  if (m_pData == NULL)
  {
    m_Capacity = 0;
  }
}


EntityCollection::~EntityCollection()
{ 
  free(m_pData);
  m_pData = NULL;
}


bool EntityCollection::Push(EntityBase* pEntity)
{ 
  if (m_Size >= m_Capacity)
  {
    if(resize() == false)
    {
      return false;
    }
  }

  *(m_pData + m_Size++) = pEntity;
  return true;
}

EntityBase* EntityCollection::Pop()
{
  return *(m_pData + --m_Size);
}


void EntityCollection::Set(size_t index, EntityBase* pEntity)
{ 
  while (index >= m_Size)
  {
    this->Push(0);
  }

  *(m_pData + index) = pEntity;
}


const EntityBase* EntityCollection::operator[](size_t index) const
{ 
  return *(m_pData + index);
}

size_t EntityCollection::Size() const
{ 
  return m_Size;
}

size_t EntityCollection::Capacity() const
{ 
  return m_Capacity;
}

bool EntityCollection::resize()
{
  if(m_Capacity == 0)
  {
    // If initial capacity could't be allocated the next allocation
    // would also fail
    return false;
  }

  size_t capacity = m_Capacity*GROWTH_FACTOR;
  EntityBase **tmp = (EntityBase**)realloc(m_pData, capacity * sizeof(*m_pData));
  if (!tmp)
  {
    return false;
  }

  m_pData = tmp;
  m_Capacity = capacity;
  return true;
}
