# kuzne
Experimental kernel development

# APIC and IPIs (Inter-Processor Interrupts): 
The APIC (Advanced Programmable Interrupt Controller) can be used to send IPIs to other cores. 
This is how one core can signal another to execute a task.


# SIPI (Startup Inter-Processor Interrupt): 
The lapic_send_sipi function sends a SIPI to other cores. This is typically used to start up other cores. 
The destination cores, upon receiving a SIPI, will start executing from a provided memory address.


# Initialize and Start APs (Application Processors):
Send an Inter-Processor Interrupt (IPI) to wake up the APs. This involves writing to the APIC's Interrupt Command Register (ICR) to issue a Startup IPI (SIPI).
Each AP starts execution at a predefined address specified by the SIPI.
