/***********************************************************************************************************************************
Key Value Handler
***********************************************************************************************************************************/
#ifndef COMMON_TYPE_KEYVALUE_H
#define COMMON_TYPE_KEYVALUE_H

/***********************************************************************************************************************************
KeyValue object
***********************************************************************************************************************************/
#define KEY_VALUE_TYPE                                              KeyValue
#define KEY_VALUE_PREFIX                                            kv

typedef struct KeyValue KeyValue;

#include "common/type/variantList.h"

/***********************************************************************************************************************************
Functions
***********************************************************************************************************************************/
KeyValue *kvNew(void);
KeyValue *kvDup(const KeyValue *source);
KeyValue *kvAdd(KeyValue *this, const Variant *key, const Variant *value);
const VariantList *kvKeyList(const KeyValue *this);
KeyValue *kvMove(KeyValue *this, MemContext *parentNew);
KeyValue *kvPut(KeyValue *this, const Variant *key, const Variant *value);
KeyValue *kvPutKv(KeyValue *this, const Variant *key);
const Variant *kvGet(const KeyValue *this, const Variant *key);
bool kvKeyExists(const KeyValue *this, const Variant *key);
VariantList *kvGetList(const KeyValue *this, const Variant *key);
void kvFree(KeyValue *this);

/***********************************************************************************************************************************
Macros for function logging
***********************************************************************************************************************************/
#define FUNCTION_LOG_KEY_VALUE_TYPE                                                                                                \
    KeyValue *
#define FUNCTION_LOG_KEY_VALUE_FORMAT(value, buffer, bufferSize)                                                                   \
    objToLog(value, "KeyValue", buffer, bufferSize)

#endif
