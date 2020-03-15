.extern context_handler

.global get_eip
get_eip:
    mov (%esp), %eax
    ret

.global get_context
get_context:
    pusha
    push %eax

    mov (%esp), %eax
    call context_handler

    pop %eax
    popa

    ret

.global context_switch
context_switch:

    mov 0x04(%esp), %ebp
    mov 0x04(%ebp), %ecx
    mov 0x08(%ebp), %edx
    mov 0x0c(%ebp), %ebx
    mov 0x10(%ebp), %esi
    mov 0x14(%ebp), %edi

    #movl 0x04(%esp), %ecx       	 # Point ecx to previous registers
    #movl (%esp), %eax             # Get return address
    #movl %eax, 0x00(%ecx)         # Save it as eip
    #movl %ebx, 0x04(%ecx)         # Save ebx
    #movl %edi, 0x08(%ecx)         # Save edi
    #movl %esi, 0x0c(%ecx)         # Save esi
    #movl %ebp, 0x10(%ecx)         # Save ebp
    #add $4, %esp                  # Move esp in state corresponding to eip
    #movl %esp, 0x14(%ecx)         # Save esp
    #pushf                         # Push flags
    #pop  0x18(%ecx)  	           # ...and save them

    #movl 0x04(%esp), %ecx        	# Point ecx to next registers
    #movl 0x04(%ecx), %ebx         # Restore ebx
    #movl 0x08(%ecx), %edi         # Restore edi
    #movl 0x14(%ecx), %esi         # Restore esp
    #movl 0x10(%ecx), %ebp         # Restore ebp
    #movl 0x14(%ecx), %esp         # Restore esp
    #push 0x18(%ecx)  	            # Push saved flags
    #popf                        	# Restore flags
    #movl 0x00(%ecx), %eax         # Get eip
    #push %eax                  	  # Restore it as return address

    ret
