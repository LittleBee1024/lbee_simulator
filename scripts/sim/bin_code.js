const vBinCode = {
   props: ['binCode'],
   methods: {
      run() {
         console.log("Run")
      },
   },
   template: `
      <el-form label-position="top">
         <el-form-item id="bin-code" label="Y86-64 Bin Code">
            <el-input
               v-model="binCode"
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

export default vBinCode;
