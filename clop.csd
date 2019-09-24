<CsoundSynthesizer>
<CsOptions>
--opcode-lib=./libclop.dylib
</CsOptions>
<CsInstruments>
ksmps = 128
0dbfs = 1
instr 1

Skernel = {{
kernel void process(global float *out, global const float *in) {
  int i = get_global_id(0);
  out[i] = in[i]*in[i];
  }
}}

Skerneld = {{
kernel void process(global double *out, global const double *in) {
  int i = get_global_id(0);
  out[i] = in[i]*in[i];
  }
}}


idev = 0
ain oscil 0.5, 440
;asig clopf Skernel, idev, ksmps, ain
asig clopd Skerneld, idev, ksmps, ain
  out asig

endin

</CsInstruments>
<CsScore>
i1 0 100
</CsScore>
</CsoundSynthesizer>