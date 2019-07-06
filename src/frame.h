#ifndef FRAME_H_INCLUDED
#define FRAME_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void free_frame(page_t *page);



#ifdef __cplusplus
}
#endif

#endif // FRAME_H_INCLUDED
