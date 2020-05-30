#ifndef ENTITY_COLLECTION_H
#define ENTITY_COLLECTION_H

#include <stddef.h>
#include "EntityBase.h"

#define MIN_CAPACITY 10
#define GROWTH_FACTOR 2

/**
 * A collection to create and store multiple entities of the same type.
 * 
 * Internally it is designed as a dynamically growing array.
 *
 *
 * @author Uwe Rosner
 * @date 01/03/2020
 */
class EntityCollection
{
public:
  EntityCollection();
  virtual ~EntityCollection();

  bool Push(EntityBase* pEntity);
  EntityBase* Pop();

  void Set(size_t index, EntityBase* pEntity);

  const EntityBase* operator[](size_t index) const;

  size_t Size() const;

  size_t Capacity() const;

private:
    size_t m_Capacity;
    size_t m_Size;
    EntityBase** m_pData;

    bool resize();
};

#endif
