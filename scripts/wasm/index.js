const vWasm = {
   data() {
      return {
         input: '',
         output: ''
      }
   },
   methods: {
      toUppercase() {
         this.output = UTF8ToString(Module._ToUppercase(allocateUTF8OnStack(this.input)))
      },
      clear() {
         this.input = ''
         this.output = ''
      }
   },
   template: `
      <div>
         <el-form label-position="top">
            <el-form-item label="Code">
               <el-input
                  v-model="input"
                  type="textarea"
                  placeholder="Please input"
                  :rows="10"
               />
            </el-form-item>
            <el-form-item id="form-button">
               <el-button type="primary" plain @click="toUppercase">Submit</el-button>
               <el-button type="warning" plain @click="clear">Clear</el-button>
            </el-form-item>
         </el-form>

         <el-divider>Result</el-divider>

         <div class="result">
            <p> {{output}} </p>
         </div>
      </div>
   `
}

export default vWasm;
