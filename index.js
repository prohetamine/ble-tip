const AppChannel            = require('node-mermaid/store/app-channel')()
    , AppTransportChannel   = require('node-mermaid/store/app-transport-channel')()
    , parser                = require('node-mermaid/parser')

AppChannel.on('connect', () => {
  AppTransportChannel.on('connect', () => {

    AppChannel.on('reload', () => {
      AppTransportChannel.writeData({
        type: 'reload'
      })
    })

    AppChannel.on('data', data => {
      const bleTip = data => {
        if (data.isEasyData && data.easyData.events.isTokens) {
          const tokenCount = data.easyData.tokenCount

          AppTransportChannel.writeData({
            type: 'tip',
            data: {
              count: tokenCount
            }
          })
        }
      }

      parser.Chaturbate(data, bleTip)
      parser.xHamsterLive(data, bleTip)
      parser.Stripchat(data, bleTip)
      parser.BongaCams(data, bleTip)
    })
  })
})
