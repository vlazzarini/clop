<CsoundSynthesizer>
<CsOptions>
--opcode-lib=./libclop.dylib
</CsOptions>
<CsInstruments>

ksmps = 64
0dbfs = 1

instr 1

/* float-based kernel for clopf */
Skernel = {{
kernel void process(global float *out, global const float *in) {
  int i = get_global_id(0);
  out[i] = in[i]*in[i];
  }
}}

idev = 0
ain oscil 0.5, 440
asig clopf Skernel, idev, ksmps, ain
  out asig

endin

instr 2

/* double-based kernel for clopd */
Skerneld = {{
kernel void process(global double *out, global const double *in) {
  int i = get_global_id(0);
  out[i] = in[i]*in[i];
  }
}}

idev = 
ain oscil 0.5, 440
asig clopd Skerneld, idev, ksmps, ain
  out asig

endin

</CsInstruments>
<CsScore>
i1 0 1
</CsScore>
</CsoundSynthesizer>