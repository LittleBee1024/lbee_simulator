const vBinCode = {
   props: ['binCode'],
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
      </el-form>
   `
}

export default vBinCode;
