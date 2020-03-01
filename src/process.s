.global copy_page_physical
copy_page_physical:
  push %ebx              # According to __cdecl, we must preserve the contents of EBX.
  pushf                  # push EFLAGS, so we can pop it and reenable interrupts
                         # later, if they were enabled anyway.
  cli                    # Disable interrupts, so we aren't interrupted.
                         # Load these in BEFORE we disable paging!
  mov 12(%esp), %ebx      # Source address
  mov 16(%esp), %ecx      # Destination address

  mov %cr0, %edx         # Get the control register...
  and $0x7fffffff, %edx  # and...
  mov %edx, %cr0          # Disable paging.

  mov $1024, %edx        # 1024*4bytes = 4096 bytes to copy

.loop:
  mov (%ebx), %eax       # Get the word at the source address
  mov %eax, (%ecx)       # Store it at the dest address
  add $4, %ebx           # Source address += sizeof(word)
  add $4, %ecx           # Dest address += sizeof(word)
  dec %edx               # One less word to do
  jnz .loop

  mov %cr0, %edx         # Get the control register again
  or  $0x80000000, %edx  # and...
  mov %edx, %cr0         # Enable paging.

  popf                   # Pop EFLAGS back.
  pop %ebx               # Get the original value of EBX back.
  ret
