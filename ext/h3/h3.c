#include <ruby.h>
#include <h3/h3api.h>
#include <inttypes.h>
#include <stdio.h>

struct my_malloc {
  size_t size;
  void *ptr;
};

static void
my_malloc_free(void *p) {

}

static VALUE
my_malloc_alloc(VALUE klass) {
  VALUE obj;
  struct my_malloc *ptr;

  obj = Data_Make_Struct(klass, struct my_malloc, NULL, my_malloc_free, ptr);

  ptr->size = 0;
  ptr->ptr  = NULL;

  return obj;
}

static VALUE
my_malloc_init(VALUE self, VALUE size) {
  struct my_malloc *ptr;
  size_t requested = NUM2SIZET(size);

  if (0 == requested)
    rb_raise(rb_eArgError, "unable to allocate 0 bytes");

  Data_Get_Struct(self, struct my_malloc, ptr);

  ptr->ptr = malloc(requested);

  if (NULL == ptr->ptr)
    rb_raise(rb_eNoMemError, "unable to allocate %" PRIuSIZE " bytes", requested);

  ptr->size = requested;

  return self;
}

static VALUE
my_malloc_release(VALUE self) {
  H3Index indexed = 0x8a2a1072b59ffffL;
  // Distance away from the origin to find:
  int k = 2;

  int maxNeighboring = maxKringSize(k);
  H3Index* neighboring = calloc(maxNeighboring, sizeof(H3Index));
  kRing(indexed, k, neighboring);

  printf("Neighbors:\n");
  for (int i = 0; i < maxNeighboring; i++) {
      // Some indexes may be 0 to indicate fewer than the maximum
      // number of indexes.
      if (neighboring[i] != 0) {
          printf("%" PRIx64 "\n", neighboring[i]);
      }
  }

  free(neighboring);

  return self;
}

static VALUE geo_to_h3(VALUE self, VALUE latlonRes) {
  printf("hola: ");
  printf("Float value is %f \n", rb_num2dbl(rb_ary_entry(latlonRes, 0)));
  GeoCoord location;
  location.lat = degsToRads(rb_num2dbl(rb_ary_entry(latlonRes, 0)));
  location.lon = degsToRads(rb_num2dbl(rb_ary_entry(latlonRes, 1)));

  H3Index indexed = geoToH3(&location, NUM2INT(rb_ary_entry(latlonRes, 2)));
  printf("%" PRIx64 "\n", indexed);

  char str[20];
  sprintf(str, "%" PRIx64, indexed);
  printf("String is %s \n" , str);

  return rb_str_new2(str);
}

void
Init_h3(void) {
  VALUE cMyMalloc;

  cMyMalloc = rb_define_class("H3", rb_cObject);

  rb_define_alloc_func(cMyMalloc, my_malloc_alloc);
  rb_define_method(cMyMalloc, "initialize", my_malloc_init, 1);
  rb_define_method(cMyMalloc, "free", my_malloc_release, 0);
  rb_define_method(cMyMalloc, "geo_to_h3", geo_to_h3, 1);
}
