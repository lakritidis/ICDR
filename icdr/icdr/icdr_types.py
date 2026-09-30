import ctypes


class IndexDataConnector(ctypes.c_void_p):
    # subclassing c_void_p creates an opaque pointer type that is distinct from c_void_p,
    # and can only be instantiated as a pointer
    pass

# Connectors for Result Object and Array of Result objects
class ResultTypeConnector(ctypes.Structure):
    _fields_ = [
        ("id", ctypes.c_int),
        ("text", ctypes.c_char_p),
        ("score", ctypes.c_float),
    ]

class ResultsTypeConnector(ctypes.Structure):
    # subclassing c_void_p creates an opaque pointer type that is distinct from c_void_p,
    # and can only be instantiated as a pointer
    _fields_ = [
        ("num_results", ctypes.c_int),
        ("results", ctypes.POINTER(ResultTypeConnector)),
    ]

# Connectors for Result Object and Array of Result objects
class PairResultTypeConnector(ctypes.Structure):
    _fields_ = [
        ("left_id", ctypes.c_int),
        ("left_text", ctypes.c_char_p),
        ("right_id", ctypes.c_int),
        ("right_text", ctypes.c_char_p),
        ("score", ctypes.c_float),
    ]

class PairResultsTypeConnector(ctypes.Structure):
    # subclassing c_void_p creates an opaque pointer type that is distinct from c_void_p,
    # and can only be instantiated as a pointer
    _fields_ = [
        ("num_results", ctypes.c_int),
        ("results", ctypes.POINTER(PairResultTypeConnector)),
    ]

# Connectors for Entity Object and Array of Entity objects
class EntityTypeConnector(ctypes.Structure):
    _fields_ = [
        ("id", ctypes.c_int),
        ("code", ctypes.c_char_p),
        ("matching_records", ctypes.POINTER(ctypes.c_int)),
        ("words", ctypes.c_void_p),
        ("next", ctypes.c_void_p),
        ("num_alloc_matching_records", ctypes.c_int16),
        ("num_matching_records", ctypes.c_int16),
        ("num_alloc_words", ctypes.c_int16),
        ("num_words", ctypes.c_int16),
    ]

class EntitiesTypeConnector(ctypes.Structure):
    # subclassing c_void_p creates an opaque pointer type that is distinct from c_void_p,
    # and can only be instantiated as a pointer
    _fields_ = [
        ("num_entities", ctypes.c_int),
        ("entities", ctypes.POINTER(EntityTypeConnector)),
    ]

# Connectors for Record Object and Array of Record objects
class RecordTypeConnector(ctypes.Structure):
    _fields_ = [
        ("id", ctypes.c_int),
        ("text", ctypes.c_char_p),
        ("word_len", ctypes.c_int),
        ("uword_len", ctypes.c_int),
        ("matching_entity", ctypes.POINTER(EntityTypeConnector))
    ]

class RecordsTypeConnector(ctypes.Structure):
    # subclassing c_void_p creates an opaque pointer type that is distinct from c_void_p,
    # and can only be instantiated as a pointer
    _fields_ = [
        ("num_records", ctypes.c_int),
        ("records", ctypes.POINTER(RecordTypeConnector)),
    ]