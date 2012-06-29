#ifndef RT_MEM_H
#define RT_MEM_H


#define RT_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))
#define RT_ALIGN_SIZE  8
#define RT_NULL                         ((void *)0)
/**
 * @ingroup BasicDef
 *
 * @def RT_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. RT_ALIGN_DOWN(13, 4)
 * would return 12. 
 */
#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

/* RT-Thread basic data type definitions */
typedef signed   char                   rt_int8_t;      /**<  8bit integer type */
typedef signed   short                  rt_int16_t;     /**< 16bit integer type */
typedef signed   long                   rt_int32_t;     /**< 32bit integer type */
typedef unsigned char                   rt_uint8_t;     /**<  8bit unsigned integer type */
typedef unsigned short                  rt_uint16_t;    /**< 16bit unsigned integer type */
typedef unsigned long                   rt_uint32_t;    /**< 32bit unsigned integer type */
typedef int                             rt_bool_t;      /**< boolean type */

/* 32bit CPU */
typedef long                            rt_base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   rt_ubase_t;     /**< Nbit unsigned CPU related data type */

typedef rt_base_t                       rt_err_t;       /**< Type for error number      */
typedef rt_uint32_t                     rt_time_t;      /**< Type for time stamp        */
typedef rt_uint32_t                     rt_tick_t;      /**< Type for tick count        */
typedef rt_base_t                       rt_flag_t;      /**< Type for flags             */
typedef rt_ubase_t                      rt_size_t;      /**< Type for size number       */
typedef rt_ubase_t                      rt_dev_t;       /**< Type for device            */
typedef rt_base_t                       rt_off_t;       /**< Type for offset            */
void rt_system_heap_init(void *begin_addr, void *end_addr);

void *rt_malloc(rt_size_t nbytes);
void rt_free(void *ptr);
void *rt_realloc(void *ptr, rt_size_t nbytes);
void *rt_calloc(rt_size_t count, rt_size_t size);
void *rt_malloc_align(rt_size_t size, rt_size_t align);
void rt_free_align(void *ptr);

void rt_memory_info(rt_uint32_t *total, rt_uint32_t *used, rt_uint32_t *max_used);
#ifdef RT_USING_HOOK
#define RT_OBJECT_HOOK_CALL(func, argv) \
    do { if ((func) != RT_NULL) func argv; } while (0)
#else
#define RT_OBJECT_HOOK_CALL(func, argv) 
#endif


#endif
