"""
Part of the high-level python bindings for Kopano

Copyright 2005 - 2016 Zarafa and its licensors (see LICENSE file)
Copyright 2016 - Kopano and its licensors (see LICENSE file)
"""

try:
    import cPickle as pickle
except ImportError:
    import pickle

try:
    from functools import lru_cache
except ImportError:
    from .lru_cache import lru_cache

try:
    from StringIO import StringIO
except ImportError:
    # We only need it for Python 2
    pass

import codecs
import io
import sys

_BIN_ENCODING = 'hex'

# Python 3
if sys.hexversion >= 0x03000000:
    def is_str(s):
        return isinstance(s, str)

    def pickle_load(f):
        return pickle.load(f, encoding='bytes')

    def pickle_loads(s):
        return pickle.loads(s, encoding='bytes')

    def hex(s):
        return codecs.encode(s, 'hex').upper().decode('ascii')

    def unhex(s):
        return codecs.decode(s, 'hex')

    def benc(s):
        if _BIN_ENCODING == 'hex':
            return codecs.encode(s, _BIN_ENCODING).strip().upper().decode('ascii')
        else:
            return codecs.encode(s, _BIN_ENCODING).strip().decode('ascii')

    def bdec(s):
        return codecs.decode(s, _BIN_ENCODING)

    def is_int(i):
        return isinstance(i, int)

    def is_file(f):
        return isinstance(f, io.IOBase)

    def repr(o):
        return o.__unicode__()

    def fake_unicode(s):
        return str(s)

    def decode(s):
        return s

    def encode(s):
        return s.encode()

# Python 2
else:
    def is_str(s):
        return isinstance(s, (str, unicode))

    def pickle_load(f):
        return pickle.loads(f)

    def pickle_loads(s):
        return pickle.loads(s)

    def hex(s):
        return s.encode('hex').upper()

    def unhex(s):
        return s.decode('hex')

    def benc(s):
        if _BIN_ENCODING == 'hex':
            return s.encode(_BIN_ENCODING).strip().upper()
        else:
            return s.encode(_BIN_ENCODING).strip()

    def bdec(s):
        return s.decode(_BIN_ENCODING)

    def is_int(i):
        return isinstance(i, (int, long))

    def is_file(f):
        return isinstance(f, file) or isinstance(f, StringIO)

    def encode(s):
        # sys.stdout can be StringIO (nosetests)
        return s.encode(getattr(sys.stdout, 'encoding', 'utf8') or 'utf8')

    def decode(s):
        return s.decode(getattr(sys.stdin, 'encoding', 'utf8') or 'utf8')

    def repr(o):
        return encode(unicode(o))

    def fake_unicode(u):
        return unicode(u)

def set_bin_encoding(encoding):
    global _BIN_ENCODING
    _BIN_ENCODING = encoding
