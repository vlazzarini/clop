<CsoundSynthesizer>
<CsOptions>
--opcode-lib=./libclop.dylib
</CsOptions>
<CsInstruments>

ksmps = 64
0dbfs = 1

kernel@global:S = {{
   kernel void process(global float *out, 
	global const float *in) {
     int i = get_global_id(0);
     out[i] = in[i]*in[i];
   }
}}

clproc:InstrDef = create({{
 dev:i = 0
 sig:a = oscili(p4, p5)
 sig = clopf(kernel, dev, ksmps, sig)
 out(sig)
}})

schedule(clproc, 0, 10, 0dbfs, 100)

</CsInstruments>
<CsScore>
f0 10
</CsScore>
</CsoundSynthesizer>