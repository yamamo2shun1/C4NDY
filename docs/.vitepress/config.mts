import {
	defineConfig
} from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
	head:
		[
			[
				"script", 
				{ async: "", src: "https://www.googletagmanager.com/gtag/js?id=G-EHFBVRHX4Q" }
			],
			[
				"script", 
				{}, 
				`window.dataLayer = window.dataLayer || [];
				function gtag() { dataLayer.push(arguments); } 
				gtag('js', new Date()); 
				gtag('config', 'G-EHFBVRHX4');`
			]
		],

	locales:
	{
		root:
		{
			label:
				'English',
			lang: 'en',
			title: "C4NDY KeyVLM / STK",
			description: "C4NDY Project Site",
			themeConfig:
			{
				// https://vitepress.dev/reference/default-theme-config
				nav:
					[{ text: 'Home', link: '/' },
					],

				sidebar: [
					{
						text: 'Tutorial',
						items: [
							{ text: 'Features', link: '/features' },
							{ text: 'Specifications', link: '/specifications' },
							{ text: 'Side Panel I/O', link: '/side-panel' },
							{ text: 'Example Layout', link: '/example-layout' },
							{ text: 'How to remap', link: '/keymap-configurator' },
							{ text: 'Firmware Update', link: '/firmware-update' },
						]
					}
				],

				socialLinks: [
					{ icon: 'github', link: 'https://github.com/yamamo2shun1/C4NDY' }
				]
			}
		}
		,
		jp:
		{
			label:
				'Japanese',
			lang: 'jp',
			title: "C4NDY KeyVLM / STK",
			description: "C4NDY Project Site",
			themeConfig:
			{
				// https://vitepress.dev/reference/default-theme-config
				nav:
					[{ text: 'Home', link: '/jp' },
					],

				sidebar: [
					{
						text: 'チュートリアル',
						items: [
							{ text: '特徴', link: '/jp/features' },
							{ text: '仕様', link: '/jp/specifications' },
							{ text: 'サイドパネルI/O', link: '/jp/side-panel' },
							{ text: 'レイアウト例', link: '/jp/example-layout' },
							{ text: 'キーボードのリマップ', link: '/jp/keymap-configurator' },
							{ text: 'ファームウェアの更新', link: '/jp/firmware-update' },
						]
					}
				],

				socialLinks: [
					{ icon: 'github', link: 'https://github.com/yamamo2shun1/C4NDY' }
				]
			}
		}
	}
})
