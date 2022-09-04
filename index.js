import vRoot from './scripts/index.js'

const app = Vue.createApp({})

for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
   app.component(key, component)
}

app.use(ElementPlus)
app.component('root', vRoot)
app.mount('#app')
