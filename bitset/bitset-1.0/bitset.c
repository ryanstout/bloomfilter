#include "ruby.h"
#include "string.h"

#define BIGNUM

static VALUE cBitSet;
static ID id_new, id_first, id_end;

struct BitSet {
  int len;
  unsigned char *ptr;
};

int ZERO_TABLE[] = {
  8, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

int ONE_TABLE[] = {
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 8
};

int MAX_TABLE[] = {
  0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

int COUNT_TABLE[] = {
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
};


void bs_resize(struct BitSet *bits, int len) {
  int arylen;
  int arybytes, arymod;
  int bytes, mod;
  arylen   = bits->len;
  arybytes = (arylen + 7) / 8;
  arymod   =  arylen      % 8;
  bytes    = (len + 7)    / 8;
  mod      =  len         % 8;

  if (len==arylen) {
    return;
  } else if (len<arylen) {
    bits->ptr = REALLOC_N(bits->ptr, unsigned char, bytes);
    bits->len = len;
    if (mod>0) {
      bits->ptr[bytes-1] &= (1<<mod)-1;
    }
  } else if (len>arylen) {
    bits->ptr = REALLOC_N(bits->ptr, unsigned char, bytes);
    memset(bits->ptr+arybytes, 0, bytes-arybytes);
    bits->len = len;
  }
}



int bs_get(struct BitSet *bits, int index) {
  int byte_offset;
  int bit_offset;
  unsigned char c;
  int bit;
  
  if (index >= bits->len) {
    bit = 0;
  } else {
    byte_offset = index / 8;
    bit_offset  = index % 8;
    
    c = bits->ptr[byte_offset];
    
    if (c & (1 << bit_offset)) {
      bit = 1;
    } else {
      bit = 0;
    }
  }
  
  return bit;
}


void bs_set(struct BitSet *bits, int index, int bit) {
  int byte_offset;
  int bit_offset;
  unsigned char c;
  
  if (index >= bits->len) {
    bs_resize(bits, index+1);
  }
  
  byte_offset = index / 8;
  bit_offset  = index % 8;
  
  c = bits->ptr[byte_offset];
  
  if (bit) {
    c |= (1 << bit_offset);
  } else {
    c &= ~(1 << bit_offset);
  }
  
  bits->ptr[byte_offset] = c;
  
}

void bs_sets(struct BitSet *bits, int first, int end, int bit) {
  int first_offset;
  int end_offset;
  int first_mod;
  int end_mod;
  int bytes;
  int i;
  unsigned char c;
  
  if (bits->len < end) {
    bs_resize(bits, end);
  }
  
  first_offset = first / 8;
  first_mod    = first % 8;
  end_offset   = end   / 8;
  end_mod      = end   % 8;
  bytes = end_offset - first_offset + 1;
  
  if (first_mod==0 && end_mod==7) {
    memset(bits->ptr+first_offset, bit ? 0xff : 0x00, bytes);
    return;
  }
  
  if (end-first < 16) {
    for(i=first; i<=end; i++) {
      bs_set(bits, i, bit);
    }
    return;
  }
  
  if (first_mod != 0) {
    c = bits->ptr[first_offset];
    if (bit) {
      c |= ~((1 << first_mod) - 1);
    } else {
      c &= ((1 << first_mod) - 1);
    }
    bits->ptr[first_offset] = c;
    first_offset++;
    bytes--;
  }
  
  if (end_mod != 7) {
    c = bits->ptr[end_offset];
    if (bit) {
      c |= ((2 << end_mod) - 1);
    } else {
      c &= ~((2 << end_mod) - 1);
    }
    bits->ptr[end_offset] = c;
    bytes--;
  }
  
  if (bytes>0) {
    memset(bits->ptr+first_offset, bit ? 0xff : 0x00, bytes);
  }
  
  return;
}


void bs_fill(VALUE obj, struct BitSet *bits, int fill) {
  int i;
  VALUE vfirst, vend;
  int first, end;
  switch(TYPE(obj)) {
  case T_FIXNUM:
    bs_set(bits, FIX2INT(obj), fill);
    break;
  case T_ARRAY:
    for(i=0; i< (RARRAY(obj)->len); i++) {
      bs_fill( (RARRAY(obj)->ptr[i]), bits, fill);
    }
    break;
  default:
    if (rb_class_of(obj)==rb_cRange) {
      vfirst = rb_funcall(obj, id_first, 0);
      vend   = rb_funcall(obj, id_end, 0);
      Check_Type(vfirst, T_FIXNUM);
      Check_Type(vend, T_FIXNUM);
      first = FIX2INT(vfirst);
      end   = FIX2INT(vend);
      if (rb_funcall(obj, rb_intern("exclude_end?"), 0) == Qtrue) {
        end--;
      }
      if (first>end || first<0 || end<0) {
        rb_raise(rb_eRangeError, "not valid range");
      }
      bs_sets(bits, first, end, fill);
    } else {
      rb_raise(rb_eTypeError, "not valid index");
    }
  }
}

void bs_or(struct BitSet *bits, struct BitSet *x) {
  unsigned char *ptr, *xptr;
  int bytes;
  int i;
  
  if (bits->len < x->len) {
    bs_resize(bits, x->len);
  }
  
  bytes = (x->len+7) / 8;
  ptr   = bits->ptr;
  xptr  = x->ptr;
  for(i=0; i<bytes; i++) {
    *ptr++ |= *xptr++;
  }
  
}

void bs_not(struct BitSet *bits) {
  unsigned char *ptr;
  unsigned char c;
  int bytes;
  int mod;
  int i;
  
  bytes = (bits->len+7) / 8;
  mod   = bits->len % 8;
  ptr   = bits->ptr;
  for(i=0; i<bytes; i++) {
    c = *ptr;
    *ptr++ = ~c;
  }
  if (mod>0) {
    c = *--ptr;
    *ptr = c & ((1<<mod)-1);
  }
}

/*
  aaaA  & bbbbB = xxxX
  aaaaA & bbbB  = xxxX
*/
void bs_and(struct BitSet *sbits, struct BitSet *xbits) {
  unsigned char *sptr, *xptr;
  unsigned char sch, xch;
  int srest, xrest;
  int sbytes, xbytes;
  
  sbytes = (sbits->len + 7) / 8;
  srest  = sbits->len % 8;
  xbytes = (xbits->len + 7) / 8;
  xrest  = xbits->len % 8;
  
  sptr  = sbits->ptr;
  xptr  = xbits->ptr;
  while(sbytes && xbytes) {
    *sptr++ &= *xptr++;
    sbytes--;
    xbytes--;
  }
  
  while(sbytes) {
    if (sbytes) {
      sch = *sptr;
      if (--sbytes == 0) {
        sch &= (1<<srest)-1;
      }
    } else {
      sch = 0;
    }
    if (xbytes) {
      xch = *xptr++;
      if (--xbytes == 0) {
        xch &= (1<<xrest)-1;
      }
    } else {
      xch = 0;
    }
    *sptr++ = sch & xch;
  }
  
}

void bs_xor(struct BitSet *bits, struct BitSet *x) {
  unsigned char *ptr, *xptr;
  int bytes;
  int i;
  
  if (bits->len < x->len) {
    bs_resize(bits, x->len);
  }
  
  bytes = (x->len+7) / 8;
  ptr   = bits->ptr;
  xptr  = x->ptr;
  for(i=0; i<bytes; i++) {
    *ptr++ ^= *xptr++;
  }
  
}

int bs_max(struct BitSet *bits) {
  int bytes;
  int index;
  int pos;
  unsigned char *ptr, c;

  bytes = (bits->len+7) / 8;
  index = (bytes-1)     * 8;
  ptr  =  bits->ptr + bytes - 1;
  
  while(index>=0) {
    c = *ptr--;
    pos = MAX_TABLE[c];
    if (pos) {
      index = index + pos - 1;
      return index;
    }
    index -= 8;
  }
  
  return -1;
}

int bs_min(struct BitSet *bits) {
  int index;
  int len;
  int pos;
  unsigned char *ptr, c;
  
  index = 0;
  ptr   = bits->ptr;
  len   = bits->len;
  while(index<len) {
    c = *ptr++;
    pos = ZERO_TABLE[c];
    if (pos<8) {
      index = index + pos;
      return index;
    }
    index += 8;
  }
  
  return -1;
}







int to_bit(VALUE obj) {
  int bit;
  
  switch(TYPE(obj)) {
  case T_NIL:
  case T_FALSE:
    bit = 0;
    break;
  case T_FIXNUM:
    if (FIX2INT(obj)==0) {
      bit = 0;
    } else {
      bit = 1;
    }
    break;
  default:
    bit = 1;
  }
  
  return bit;
}

void bits_free(struct BitSet *bits) {
  ruby_xfree(bits->ptr);
}

static VALUE bits_s_new(int argc, VALUE *argv, VALUE self) {
  struct BitSet *bits;
  VALUE arg;
  VALUE obj;
  int len;
  int bytes;
  
  obj = Data_Make_Struct(self, struct BitSet, NULL, bits_free, bits);
  
  if (argc>0) {
    arg = argv[0];
  } else {
    arg = INT2FIX(1);
  }
  
  switch(TYPE(arg)) {
  case T_FIXNUM:
/* Was - ryan */
  case T_BIGNUM:
    #ifndef BIGNUM
    len = FIX2INT(arg);
    #else
    len = NUM2INT(arg);
    #endif

    if (len<1) {
      rb_raise(rb_eArgError, "array size");
    }

    bytes = (len+7)/8; 

    bits->len = len;
    bits->ptr = ALLOC_N(unsigned char, bytes);
    memset(bits->ptr, 0, bytes);
    break;
  case T_STRING:
    bytes = RSTRING(arg)->len;
    bits->len = bytes*8;
    bits->ptr = ALLOC_N(unsigned char, bytes);
    memcpy(bits->ptr, RSTRING(arg)->ptr, bytes);
    break;
  default:
    rb_raise(rb_eArgError, "not valid value");
  }
  
  return obj;
}

static VALUE bits_s_from_bin(VALUE self, VALUE arg) {
  struct BitSet *bits;
  VALUE retobj;
  unsigned char ch, *ptr, *bptr;
  int len;
  int bytes;
  int bitcnt;
  
  Check_Type(arg, T_STRING);
  
  len = RSTRING(arg)->len;
  if (len<1) {
    rb_raise(rb_eArgError, "array size");
  }
  bytes = (len+7)/8;
  
  retobj = Data_Make_Struct(self, struct BitSet, NULL, bits_free, bits);
  bits->len = len;
  bits->ptr = ALLOC_N(unsigned char, bytes);
  memset(bits->ptr, 0, bytes);
  
  ch = 0;
  bitcnt = 0;
  ptr = (unsigned char *)RSTRING(arg)->ptr;
  bptr = bits->ptr;
  while(len--) {
    switch(*ptr++) {
    case '0':
    case '-':
    case 'f':
    case 'F':
    case 'N':
      break;
    default:
      ch |= 1 << bitcnt;
    }
    bitcnt++;
    if (bitcnt==8) {
      *bptr++ = ch;
      ch = 0;
      bitcnt = 0;
    }
  }
  if (bitcnt) {
    *bptr++ = ch;
  }
  
  return retobj;
}


static VALUE bits_length(VALUE self) {
  struct BitSet *bits;
  
  Data_Get_Struct(self, struct BitSet, bits);
  
  return INT2FIX(bits->len);
}

static VALUE bits_bytes(VALUE self) {
  struct BitSet *bits;
  
  Data_Get_Struct(self, struct BitSet, bits);
  
  return INT2FIX((bits->len+7)/8);
}

static VALUE bits_resize(VALUE self, VALUE obj) {
  struct BitSet *bits;
  int len;
  
  /* Was - ryan  */
  #ifndef BIGNUM
  Check_Type(obj, T_FIXNUM);
  len = FIX2INT(obj);
  #else

  len = NUM2INT(obj);
  #endif
  if (len<1) {
    rb_raise(rb_eArgError, "array size");
  }
  
  Data_Get_Struct(self, struct BitSet, bits);
  bs_resize(bits, len);
  
  return self;
}

static VALUE bits_to_s(VALUE self) {
  struct BitSet *bits;
  unsigned char *ptr;
  int i;
  VALUE str;
  
  Data_Get_Struct(self, struct BitSet, bits);
  str = rb_str_new(0, bits->len);
  
  ptr = (unsigned char *)RSTRING(str)->ptr;
  for(i=0; i<bits->len; i++) {
    *ptr++ = bs_get(bits, i) ? '1' : '0';
  }
  
  return str;
}

static VALUE bits_dup(VALUE self) {
  struct BitSet *orig;
  struct BitSet *dups;
  VALUE obj;
  int bytes;
  
  Data_Get_Struct(self, struct BitSet, orig);
  
  obj = Data_Make_Struct(CLASS_OF(self), struct BitSet, NULL, bits_free, dups);
  dups->len = orig->len;
  bytes = (dups->len+7)/8;
  dups->ptr = ALLOC_N(unsigned char, bytes);
  memcpy(dups->ptr, orig->ptr, bytes);
  
  return obj;
}

static VALUE bits_clear(VALUE self) {
  struct BitSet *bits;
  
  Data_Get_Struct(self, struct BitSet, bits);
  memset(bits->ptr, 0, (bits->len+7)/8);
  
  return self;
}

static VALUE bits_get(VALUE self, VALUE vidx) {
  struct BitSet *bits;
  int index;
  int bit;
  
  Data_Get_Struct(self, struct BitSet, bits);
  
/* Ryan - was */
  #ifndef BIGNUM
  Check_Type(vidx, T_FIXNUM);
  index = FIX2INT(vidx);

  #else
  index = NUM2INT(vidx);
  #endif

  if (index<0) {
    rb_raise(rb_eRangeError, "index range");
  }
  
  bit = bs_get(bits, index);
  
  return INT2FIX(bit);
}


static VALUE bits_set(VALUE self, VALUE vidx, VALUE vobj) {
  struct BitSet *bits;
  int index;
  int bit;
  
  Data_Get_Struct(self, struct BitSet, bits);
  
/* Was - ryan */
  #ifndef BIGNUM
  Check_Type(vidx, T_FIXNUM);
  index = FIX2INT(vidx);

  #else
  index = NUM2INT(vidx);
  #endif

  if (index<0) {
    rb_raise(rb_eRangeError, "index range");
  }
  
  bit = to_bit(vobj);
  
  bs_set(bits, index, bit);
  
  return self;
}

static VALUE bits_on(int argc, VALUE *argv, VALUE self) {
  struct BitSet *bits;
  int i;
  Data_Get_Struct(self, struct BitSet, bits);

  for(i=0; i<argc; i++) {
    bs_fill(argv[i], bits, 1);
  }
  
  return self;
}

static VALUE bits_off(int argc, VALUE *argv, VALUE self) {
  struct BitSet *bits;
  int i;
  
  Data_Get_Struct(self, struct BitSet, bits);
  
  for(i=0; i<argc; i++) {
    bs_fill(argv[i], bits, 0);
  }
  
  return self;
}

static VALUE bits_or(VALUE self, VALUE other) {
  VALUE obj;
  struct BitSet *bits, *rbits, *xbits;
  int bytes;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  
  obj = Data_Make_Struct(CLASS_OF(self), struct BitSet, NULL, bits_free, rbits);
  rbits->len = bits->len;
  bytes      = (rbits->len+7)/8;
  rbits->ptr = ALLOC_N(unsigned char, bytes);
  memcpy(rbits->ptr, bits->ptr, bytes);
  
  Data_Get_Struct(other, struct BitSet, xbits);
  bs_or(rbits, xbits);
  
  return obj;
}

static VALUE bits_not(VALUE self) {
  VALUE obj;
  struct BitSet *bits, *rbits;
  int bytes;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  
  obj = Data_Make_Struct(CLASS_OF(self), struct BitSet, NULL, bits_free, rbits);
  rbits->len = bits->len;
  bytes      = (rbits->len+7)/8;
  rbits->ptr = ALLOC_N(unsigned char, bytes);
  memcpy(rbits->ptr, bits->ptr, bytes);
  
  bs_not(rbits);
  
  return obj;
}

static VALUE bits_and(VALUE self, VALUE other) {
  VALUE obj;
  struct BitSet *bits, *rbits, *xbits;
  int bytes;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  
  obj = Data_Make_Struct(CLASS_OF(self), struct BitSet, NULL, bits_free, rbits);
  rbits->len = bits->len;
  bytes      = (rbits->len+7)/8;
  rbits->ptr = ALLOC_N(unsigned char, bytes);
  memcpy(rbits->ptr, bits->ptr, bytes);
  
  Data_Get_Struct(other, struct BitSet, xbits);
  bs_and(rbits, xbits);
  
  return obj;
}

static VALUE bits_xor(VALUE self, VALUE other) {
  VALUE obj;
  struct BitSet *bits, *rbits, *xbits;
  int bytes;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  
  obj = Data_Make_Struct(CLASS_OF(self), struct BitSet, NULL, bits_free, rbits);
  rbits->len = bits->len;
  bytes      = (rbits->len+7)/8;
  rbits->ptr = ALLOC_N(unsigned char, bytes);
  memcpy(rbits->ptr, bits->ptr, bytes);
  
  Data_Get_Struct(other, struct BitSet, xbits);
  bs_xor(rbits, xbits);
  
  return obj;
}

static VALUE bits_minus(VALUE self, VALUE other) {
  VALUE obj;
  struct BitSet y;
  struct BitSet *bits, *rbits, *xbits, *ybits;
  int bytes;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  Data_Get_Struct(other, struct BitSet, xbits);
  
  obj = Data_Make_Struct(CLASS_OF(self), struct BitSet, NULL, bits_free, rbits);
  rbits->len = bits->len;
  bytes      = (rbits->len+7)/8;
  rbits->ptr = ALLOC_N(unsigned char, bytes);
  memcpy(rbits->ptr, bits->ptr, bytes);

  ybits = &y;
  ybits->len = xbits->len;
  bytes      = (xbits->len+7)/8;
  ybits->ptr = ALLOC_N(unsigned char, bytes);
  memcpy(ybits->ptr, xbits->ptr, bytes);
  if (ybits->len < rbits->len) {
    bs_resize(ybits, rbits->len);
  }
  
  bs_not(ybits);
  bs_and(rbits, ybits);
  
  return obj;
}

static VALUE bits_cmp(VALUE self, VALUE other) {
  struct BitSet *sbits, *obits;
  int smax,omax,bytes;
  unsigned char *sptr,*optr, sc, oc;
  
  Data_Get_Struct(self,  struct BitSet, sbits);
  Data_Get_Struct(other, struct BitSet, obits);

  smax = bs_max(sbits);
  omax = bs_max(obits);
  if (smax < omax) {
    return INT2FIX(-1);
  } else if (smax > omax) {
    return INT2FIX(1);
  }
  
  if (smax < 0) {
    return INT2FIX(0);
  }
  
  bytes = (smax+7)/8;
  sptr = sbits->ptr + bytes - 1;
  optr = obits->ptr + bytes - 1;
  while(bytes--) {
    sc = *sptr--;
    oc = *optr--;
    if (sc<oc) {
      return INT2FIX(-1);
    } else if (sc > oc) {
      return INT2FIX(1);
    }
  }
  
  return INT2FIX(0);
}


static VALUE bits_zero(VALUE self) {
  struct BitSet *bits;
  unsigned char *ptr;
  int bytes;
  int i;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  bytes      = (bits->len+7)/8;

  
  ptr = bits->ptr;
  for(i=0 ; i<bytes; i++) {
    if (*ptr++ != 0) {
      return Qfalse;
    }
  }
  
  return Qtrue;
}

static VALUE bits_nonzero(VALUE self) {
  struct BitSet *bits;
  unsigned char *ptr;
  int bytes;
  int i;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  bytes      = (bits->len+7)/8;
  
  ptr = bits->ptr;
  for(i=0 ; i<bytes; i++) {
    if (*ptr++ != 0) {
      return Qtrue;
    }
  }
  
  return Qfalse;
}

static VALUE bits_max(VALUE self) {
  struct BitSet *bits;
  int index;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  
  index = bs_max(bits);

  if (index>=0) {
    return INT2FIX(index);
  } else {
    return Qnil;
  }
}

static VALUE bits_min(VALUE self) {
  struct BitSet *bits;
  int index;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  
  index = bs_min(bits);
  
  if (index>=0) {
    return INT2FIX(index);
  } else {
    return Qnil;
  }
}

static VALUE bits_norm(VALUE self) {
  VALUE obj;
  struct BitSet *bits;
  int index;
  
  obj = bits_dup(self);
  Data_Get_Struct(obj,  struct BitSet, bits);
  
  index = bs_max(bits);
  if (index<0) {
    index = 1;
  } else {
    index = index + 1;
  }
  bs_resize(bits, index);
  
  return obj;
}

static VALUE bits_normx(VALUE self) {
  struct BitSet *bits;
  int index;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  
  index = bs_max(bits);
  
  index = bs_max(bits);
  if (index<0) {
    index = 1;
  } else {
    index = index + 1;
  }
  bs_resize(bits, index);
  
  return self;
}


static VALUE bits_to_ary(VALUE self) {
  VALUE ary;
  struct BitSet *bits;
  unsigned char *ptr, c;
  int bytes;
  int from, to, rest, index, len, cnt;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  bytes = (bits->len+7)/8;
  
  
  ary = rb_ary_new();
  
  ptr = bits->ptr;
  index = 0;
  len   = bits->len;
  from  = -1;
  to    = -1;
  rest  = 0;
  while(index<len) {
    if (rest==0) {
      c = *ptr++;
      rest = 8;
    }
    /* printf("%6d(%d): %02x %d\n", index, rest, c, from); */
    if (from < 0) {
      cnt = ZERO_TABLE[c];
      if (cnt>rest) {
        cnt = rest;
      }
      rest -= cnt;
      index += cnt;
      if (rest>0) {
        c = c >> cnt;
        from = index;
      }
    } else {
      cnt = ONE_TABLE[c];
      rest -= cnt;
      index += cnt;
      if (rest>0) {
        c = c >> cnt;
        to = index - 1;
        if (from==to) {
          rb_ary_push(ary, INT2FIX(from));
        } else {
          rb_ary_push(ary, rb_funcall(rb_cRange, id_new, 2, INT2FIX(from), INT2FIX(to)));
        }
        from = -1;
        to   = -1;
      }
    }
  }
  if (from >= 0) {
    to = index - 1;
    if (from==to) {
      rb_ary_push(ary, INT2FIX(from));
    } else {
      rb_ary_push(ary, rb_funcall(rb_cRange, id_new, 2, INT2FIX(from), INT2FIX(to)));
    }
  }
  
  return ary;
}

static VALUE bits_to_bytes(VALUE self) {
  struct BitSet *bits;
  int bytes;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  bytes = (bits->len+7)/8;
  
  return rb_str_new((char *)bits->ptr,bytes);
}

static VALUE bits_each(VALUE self) {
  struct BitSet *bits;
  int idx,len;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  
  len = bits->len;
  for(idx=0; idx<len; idx++) {
    if (bs_get(bits, idx)) {
      rb_yield(INT2FIX(idx));
    }
  }
  
  return self;
}

static VALUE bits_count(VALUE self) {
  struct BitSet *bits;
  unsigned char *ptr, ch;
  int count;
  int bytes;
  int i;
  
  Data_Get_Struct(self,  struct BitSet, bits);
  bytes      = (bits->len+7)/8;
  
  count = 0;
  ptr = bits->ptr;
  for(i=0 ; i<bytes; i++) {
    ch = *ptr++;
    count += COUNT_TABLE[ch];
  }
  
  return INT2FIX(count);
}

void Init_bitset() {
  cBitSet =  rb_define_class("BitSet", rb_cObject);
  rb_include_module(cBitSet, rb_mComparable);
  
  rb_define_singleton_method(cBitSet, "new", bits_s_new, -1 ); /* tested */
  rb_define_singleton_method(cBitSet, "from_bin", bits_s_from_bin, 1 ); /* tested */
  
  rb_define_method(cBitSet, "size",       bits_length,    0); /* tested */
  rb_define_method(cBitSet, "size=",      bits_resize,    1); /* tested */
  rb_define_method(cBitSet, "length",     bits_length,    0); /* tested */
  rb_define_method(cBitSet, "bytes",      bits_bytes,     0); /* tested */
  rb_define_method(cBitSet, "to_s",       bits_to_s,      0); /* tested */
  rb_define_method(cBitSet, "dup",        bits_dup,       0); /* tested */
  rb_define_method(cBitSet, "clone",      bits_dup,       0); /* tested */
  rb_define_method(cBitSet, "clear",      bits_clear,     0); /* tested */
  rb_define_method(cBitSet, "get",        bits_get,       1); /* tested */
  rb_define_method(cBitSet, "set",        bits_set,       2); /* tested */
  rb_define_method(cBitSet, "on",         bits_on,       -1); /* tested */
  rb_define_method(cBitSet, "off",        bits_off,      -1); /* tested */
  
  rb_define_method(cBitSet, "|",          bits_or,        1); /* tested */
  rb_define_method(cBitSet, "~",          bits_not,       0); /* tested */
  rb_define_method(cBitSet, "&",          bits_and,       1); /* tested */
  rb_define_method(cBitSet, "^",          bits_xor,       1); /* tested */
  rb_define_method(cBitSet, "+",          bits_or,        1); /* tested */
  rb_define_method(cBitSet, "-",          bits_minus,     1); /* tested */
  rb_define_method(cBitSet, "*",          bits_and,       1); /* tested */
  rb_define_method(cBitSet, "<=>",        bits_cmp,       1); /* tested */
  
  rb_define_method(cBitSet, "zero?",      bits_zero,      0); /* tested */
  rb_define_method(cBitSet, "nonzero?",   bits_nonzero,   0); /* tested */
  rb_define_method(cBitSet, "max",        bits_max,       0); /* tested */
  rb_define_method(cBitSet, "min",        bits_min,       0); /* tested */
  rb_define_method(cBitSet, "normalize",  bits_norm,      0); /* tested */
  rb_define_method(cBitSet, "normalize!", bits_normx,     0); /* tested */
  
  rb_define_method(cBitSet, "to_ary",     bits_to_ary,    0); /* tested */
  rb_define_method(cBitSet, "to_bytes",   bits_to_bytes,  0); /* tested */
  rb_define_method(cBitSet, "count",      bits_count,     0); /* tested */

  rb_define_method(cBitSet, "each",       bits_each,      0); /* tested */
  /* rb_include_module(cBitSet, rb_mEnumerable); */
  
  id_new   = rb_intern("new");
  id_first = rb_intern("first");
  id_end   = rb_intern("end");
}

