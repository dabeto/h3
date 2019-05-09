#include <ruby.h>
#include <h3api.h>
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
my_malloc_init(VALUE self) {
  struct my_malloc *ptr;
  size_t requested = 1024;

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

static VALUE neighbors(VALUE self){
    H3Index indexed = 0x8a2a1072b59ffffL;
      // Distance away from the origin to find:
      int k = 2;

      int maxNeighboring = maxKringSize(k);
      H3Index* neighboring = calloc(maxNeighboring, sizeof(H3Index));
      kRing(indexed, k, neighboring);
      VALUE r_array = rb_ary_new2(maxNeighboring);
      printf("Neighbors:\n");
      for (int i = 0; i < maxNeighboring; i++) {
          // Some indexes may be 0 to indicate fewer than the maximum
          // number of indexes.
          if (neighboring[i] != 0) {
            rb_ary_push(r_array, sprintf(str, "%" PRIx64, neighboring[i]));
              printf("%" PRIx64 "\n", neighboring[i]);
          }
      }

      free(neighboring);

      return r_array;
}

static VALUE geo_to_h3(VALUE self, VALUE latlonRes) {
  GeoCoord location;
  location.lat = degsToRads(rb_num2dbl(rb_ary_entry(latlonRes, 0)));
  location.lon = degsToRads(rb_num2dbl(rb_ary_entry(latlonRes, 1)));
  H3Index indexed = geoToH3(&location, NUM2INT(rb_ary_entry(latlonRes, 2)));
  char str[20];
  sprintf(str, "%" PRIx64, indexed);
  return rb_str_new2(str);
}

// GETS THE COORDINATES OF THE HEXAGON CORNERS
static VALUE h3_to_geo_boundary(VALUE self, VALUE h3) {
  VALUE h3ToString = rb_String(h3);
  char *String2CStr = StringValueCStr(h3ToString);
  H3Index indexed = stringToH3(String2CStr);
  GeoBoundary boundary;
  h3ToGeoBoundary(indexed, &boundary);

  VALUE r_array = rb_ary_new2(boundary.numVerts);
  for (int v = 0; v < boundary.numVerts; v++) {
    VALUE r_hash = rb_hash_new();
    rb_hash_aset(r_hash, rb_str_new2("lat"), rb_float_new(radsToDegs(boundary.verts[v].lat)));
    rb_hash_aset(r_hash, rb_str_new2("lon"), rb_float_new(radsToDegs(boundary.verts[v].lon)));
    rb_ary_push(r_array, r_hash);
	}
  return r_array;
}

  // GETS THE CENTER OF THE HEXAGON
  static VALUE h3_to_geo(VALUE self, VALUE h3) {
    VALUE h3ToString = rb_String(h3);
    char *String2CStr = StringValueCStr(h3ToString);
    H3Index indexed = stringToH3(String2CStr);
    GeoCoord center;
    h3ToGeo(indexed, &center);
    VALUE r_hash = rb_hash_new();
    rb_hash_aset(r_hash, rb_str_new2("lat"), rb_float_new(radsToDegs(center.lat)));
    rb_hash_aset(r_hash, rb_str_new2("lon"), rb_float_new(radsToDegs(center.lon)));
    return r_hash;
  }

void
Init_h3(void) {
  VALUE cMyMalloc;

  cMyMalloc = rb_define_class("H3", rb_cObject);

  rb_define_alloc_func(cMyMalloc, my_malloc_alloc);
  rb_define_method(cMyMalloc, "initialize", my_malloc_init, 0);
  rb_define_method(cMyMalloc, "free", my_malloc_release, 0);
  rb_define_method(cMyMalloc, "geo_to_h3", geo_to_h3, 1);
  rb_define_method(cMyMalloc, "h3_to_geo_boundary", h3_to_geo_boundary, 1);
  rb_define_method(cMyMalloc, "h3_to_geo", h3_to_geo, 1);
  rb_define_method(cMyMalloc, "neighbors", neighbors, 0);
}
