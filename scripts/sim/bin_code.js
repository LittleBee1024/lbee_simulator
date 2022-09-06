const vBinCode = {
   props: ['binCode'],
   template: `
      <el-form label-position="top">
         <el-divider>Y86-64 Binary Code</el-divider>
         <el-form-item id="bin-code">
            <el-input
               v-model="binCode"
               type="textarea"
               placeholder=""
               rows="10"
               :readonly="true"
            />
         </el-form-item>
      </el-form>
   `
}

export default vBinCode;
