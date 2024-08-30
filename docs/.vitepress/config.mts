import{
    defineConfig
} from 'vitepress'

    // https://vitepress.dev/reference/site-config
    export default defineConfig({
        // base: '/C4NDY/',
        title: "C4NDY KeyVLM / STK",
        description: "C4NDY Project Site",
        themeConfig: {
            // https://vitepress.dev/reference/default-theme-config
            nav: [
                {text: 'Home', link: '/'},
                //{text: '', link: '/markdown-examples'}
            ],

            sidebar: [
                {
                    text: 'Tutorial',
                    items: [
                        {text: 'Features', link: '/features'},
                        {text: 'Specifications', link: '/specifications'},
                        {text: 'Side Panel I/O', link: '/side-panel'},
                        {text: 'Example Layout', link: '/example-layout'},
                        {text: 'How to remap', link: '/keymap-configurator'},
                    ]
                }
            ],

            socialLinks: [
                {icon: 'github', link: 'https://github.com/yamamo2shun1/C4NDY'}
            ]
        }
    })
