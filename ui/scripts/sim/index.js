import vAsmCode from './asm_code.js'
import vBinCode from './bin_code.js'
import vLoad from './load.js'

const vSim = {
   data() {
      return {
         binCode: '',
         activeAddr: 0
      }
   },
   methods: {
      setBinCode(code) {
         this.binCode = code
      }
   },
   components: {
      'vasmcode': vAsmCode,
      'vbincode': vBinCode,
      'vload': vLoad,
   },
   template: `
      <div>
         <vasmcode @assemble="setBinCode"/>
         <vbincode :bin-code="binCode" />
         <vload :active-addr="activeAddr" />
      </div>
   `
}

export default vSim;
