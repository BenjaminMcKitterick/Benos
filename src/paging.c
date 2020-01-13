
#include "paging.h"
#include "interrupts.h"

static void page_fault();

void initialise_paging()
{
    //enable_paging();
    reg_int_handler(14, &page_fault);
}

static void page_fault()
{
    println("page fault triggered");
}
