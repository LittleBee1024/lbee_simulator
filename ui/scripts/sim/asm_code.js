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
sum:   irmovq $8,%r8 # Constant 8
irmovq $1,%r9        # Constant 1
xorq %rax,%rax       # sum = 0
andq %rsi,%rsi       # Set CC
jmp     test         # Goto test
loop:   mrmovq (%rdi),%r10   # Get *start
addq %r10,%rax       # Add to sum
addq %r8,%rdi        # start++
subq %r9,%rsi        # count--.  Set CC
test:   jne    loop  # Stop when 0
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
   emits: ['updateBinCode'],
   methods: {
      convert() {
         let code = UTF8ToString(Module._Sim_Assemble(allocateUTF8OnStack(this.asmCode)))
         this.$emit("updateBinCode", code)
      },
      reset() {
         this.asmCode = defaultCode
         // clean the cache binary code in wasm
         Module._Sim_Assemble(allocateUTF8OnStack(""))
         this.$emit("updateBinCode", "")
      }
   },
   template: `
      <el-form label-position="top">
         <el-divider>Y86-64 ASM Code</el-divider>
         <el-form-item>
            <el-input
               v-model="asmCode"
               type="textarea"
               placeholder="Please input assembly code"
               rows="10"
            />
         </el-form-item>
         <el-form-item>
            <el-button type="primary" icon="Check" @click="convert">Compile</el-button>
            <el-button type="warning" icon="RefreshLeft" @click="reset">Reset</el-button>
         </el-form-item>
      </el-form>
   `
}

export default vAsmCode;
