const defaultCode =
`# Execution begins at address 0
.pos 0
irmovq stack, %rsp   # Set up stack pointer
call main            # Execute main program
halt                 # Terminate program

# Array of 4 elements
.align 8
array:   .quad 0x000d000d000d
.quad 0x00c000c000c0
.quad 0x0b000b000b00
.quad 0xa000a000a000

main:   irmovq array,%rdi
irmovq $4,%rsi
call sum             # sum(array, 4)
ret

# long sum(long *start, long count)
# start in %rdi, count in %rsi
sum:   irmovq $8,%r8    # Constant 8
irmovq $1,%r9        # Constant 1
xorq %rax,%rax       # sum = 0
andq %rsi,%rsi       # Set CC
jmp     test         # Goto test
loop:   mrmovq (%rdi),%r10   # Get *start
addq %r10,%rax       # Add to sum
addq %r8,%rdi        # start++
subq %r9,%rsi        # count--.  Set CC
test:   jne    loop          # Stop when 0
ret                  # Return

# Stack starts here and grows to lower addresses
.pos 0x200
stack:
`

const vAsmCode = {
   data() {
      return {
         asmCode: defaultCode
      }
   },
   emits: ['assemble'],
   methods: {
      convert() {
         let code = UTF8ToString(Module._Assemble(allocateUTF8OnStack(this.asmCode)))
         this.$emit("assemble", code)
      },
      reset() {
         this.asmCode = defaultCode
         this.$emit("assemble", "")
      }
   },
   template: `
      <el-form label-position="top">
         <el-form-item label="Y86-64 ASM Code">
            <el-input
               v-model="asmCode"
               type="textarea"
               placeholder="Please input assembly code"
               rows="19"
            />
         </el-form-item>
         <el-form-item id="form-button">
            <el-button type="primary" @click="convert">Assemble</el-button>
            <el-button type="warning" @click="reset">Reset</el-button>
         </el-form-item>
      </el-form>
   `
}

const vMachineCode = {
   props: ['machineCode'],
   methods: {
      run() {
         console.log("Run")
      },
   },
   template: `
      <el-form label-position="top">
         <el-form-item id="machine-code" label="Y86-64 Machine Code">
            <el-input
               v-model="machineCode"
               type="textarea"
               placeholder=""
               rows="19"
               :readonly="true"
            />
         </el-form-item>
         <el-form-item id="form-button">
            <el-button type="primary" @click="run">Run</el-button>
         </el-form-item>
      </el-form>
   `
}

const vWasm = {
   data() {
      return {
         machineCode: ''
      }
   },
   methods: {
      setMachineCode(code) {
         this.machineCode = code
      }
   },
   components: {
      'vasmcode': vAsmCode,
      'vmachinecode': vMachineCode,
   },
   template: `
      <div>
         <el-row :gutter="20">
            <el-col :span="10">
               <vasmcode :machine-code="machineCode" @assemble="setMachineCode"/>
            </el-col>
            <el-col :span="14">
               <vmachinecode :machine-code="machineCode" />
            </el-col>
         </el-row>
      </div>
   `
}

export default vWasm;
