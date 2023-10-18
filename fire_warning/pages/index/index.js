// index.js
// 获取应用实例
const app = getApp()

Page({
  data: {
    imgUrls:[
      '/static/images/1.jpg',
      '/static/images/2.jpg',
      '/static/images/3.jpg',
      '/static/images/4.jpg',
      '/static/images/5.jpg',
    ],
    node1:'节点1',
    node2:'节点2',
    node3:'节点3',
    node4:'节点4',
    nodefacility1:'设备状态',
    nodefacility2:'设备状态',
    nodefacility3:'设备状态',
    nodefacility4:'设备状态',
  },
//页面监听时间
  onLoad() {
    let that = this;
    var interval;
    interval = setInterval(function() {
      that.OneNet_Get();
    }, 1800)
    //this.getOnenetData(),
  },
  //获取OneNet数据
  OneNet_Get: function(){
    var that=this
    let deviceid = "1099722645"
    let apikey = "7pHGlA1izOamf4dnWa20twWcbZE"
    wx.request({
      url: 'http://api.heclouds.com/devices/1099722645/datapoints', //设备ID
      method:'GET',
      header:{
        "content-type": 'application/json',
        "api-key":"7pHGlA1izOamf4dnWa20twWcbZE="//api-key
      },
      data:{
        limit:1
      },
      method :"GET",
       //获取成功
      success:function(res){
       console.log(res.data.data)
       that.setData({
         humi1:res.data.data.datastreams[17].datapoints[0].value, 
         humi2:res.data.data.datastreams[18].datapoints[0].value, 
         humi3:res.data.data.datastreams[19].datapoints[0].value,
         humi4:res.data.data.datastreams[20].datapoints[0].value,
         temp1:res.data.data.datastreams[12].datapoints[0].value,
         temp2:res.data.data.datastreams[7].datapoints[0].value,
         temp3:res.data.data.datastreams[9].datapoints[0].value,
         temp4:res.data.data.datastreams[15].datapoints[0].value,
         adc1:res.data.data.datastreams[16].datapoints[0].value,
         adc2:res.data.data.datastreams[4].datapoints[0].value,
         adc3:res.data.data.datastreams[2].datapoints[0].value,
         adc4:res.data.data.datastreams[0].datapoints[0].value,
         sun1:res.data.data.datastreams[23].datapoints[0].value,
         sun2:res.data.data.datastreams[21].datapoints[0].value,
         sun3:res.data.data.datastreams[22].datapoints[0].value,
         sun4:res.data.data.datastreams[14].datapoints[0].value,
         state1:res.data.data.datastreams[10].datapoints[0].value,
         state2:res.data.data.datastreams[8].datapoints[0].value,
         state3:res.data.data.datastreams[13].datapoints[0].value,
         state4:res.data.data.datastreams[11].datapoints[0].value,
       })   
      }
    })
  },

  //节点1设备控制
  nodeOneSwitch:function(e){
    let that = this;
    let sw = e.detail.value;  //获取switch的值，switch值为true或false
    console.log(e.detail.value) //打印switch状态值
    // 判断switch是打开还是关闭,ture为打开，反之
    if (sw==true) {

      wx.request({
        url: 'http://192.168.43.53/handle',
        header:{
          'content-type':'application/x-www-form-urlencoded',  
          'Connection':'keep-alive',
          'Accept-Language':'zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6',
        },
        method :"POST",
        data:{
          node:"1",
          state:"on"
        },
        success(res){
          console.log("设备打开")
        }
      })
    }
    // 判断switch是打开还是关闭
    else if(sw == false){
      wx.request({
        url: 'http://192.168.43.53/handle',
        header:{
         'content-type':'application/x-www-form-urlencoded',  
         'Connection':'keep-alive',
         'Accept-Language':'zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6',
        },
        method :"POST",
        data:{
          node:"1",
          state:"off"
        },
        // 请求成功后的回调
        success(res){
          console.log("设备关闭")
        }
      })
    }
  },
  //节点2设备控制
  nodeTwoSwitch:function(e){
      let that = this;
      let sw = e.detail.value;  //获取switch的值，switch值为true或false
      console.log(e.detail.value) //打印switch状态值
      // 判断switch是打开还是关闭,ture为打开，反之
      if (sw==true) {
        wx.request({
          url: 'http://192.168.43.53/handle',
          header:{
            'content-type':'application/x-www-form-urlencoded',  
            'Connection':'keep-alive',
            'Accept-Language':'zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6',
          },
          method :"POST",
          data:{
            node:"2",
            state:"on"
          },
          success(res){
            console.log("设备打开")
          }
        })
      }
      // 判断switch是打开还是关闭
      else if(sw == false){
        wx.request({
          url: 'http://192.168.43.53/handle',
          header:{
           'content-type':'application/x-www-form-urlencoded',  
           'Connection':'keep-alive',
           'Accept-Language':'zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6',
          },
          method :"POST",
          data:{
            node:"2",
            state:"off"
          },
          // 请求成功后的回调
          success(res){
            console.log("设备关闭")
          }
        })
      }
    },
  //节点3设备控制
  nodeThreeSwitch:function(e){
    let that = this;
    let sw = e.detail.value;  //获取switch的值，switch值为true或false
    console.log(e.detail.value) //打印switch状态值
    // 判断switch是打开还是关闭,ture为打开，反之
    if (sw==true) {
      wx.request({
        url: 'http://192.168.43.53/handle',
        header:{
          'content-type':'application/x-www-form-urlencoded',  
          'Connection':'keep-alive',
          'Accept-Language':'zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6',
        },
        method :"POST",
        data:{
          node:"3",
          state:"on"
        },
        success(res){
          console.log("设备打开")
        }
      })
    }
    // 判断switch是打开还是关闭
    else if(sw == false){
      wx.request({
        url: 'http://192.168.43.53/handle',
        header:{
         'content-type':'application/x-www-form-urlencoded',  
         'Connection':'keep-alive',
         'Accept-Language':'zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6',
        },
        method :"POST",
        data:{
          node:"3",
          state:"off"
        },
        // 请求成功后的回调
        success(res){
          console.log("设备关闭")
        }
      })
    }
  },
  //节点4设备控制
  nodeFourSwitch:function(e){
     let that = this;
     let sw = e.detail.value;  //获取switch的值，switch值为true或false
     console.log(e.detail.value) //打印switch状态值
      // 判断switch是打开还是关闭,ture为打开，反之
      if (sw==true) {
        wx.request({
          url: 'http://192.168.43.53/handle',
          header:{
            'content-type':'application/x-www-form-urlencoded',  
            'Connection':'keep-alive',
            'Accept-Language':'zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6',
          },
          method :"POST",
          data:{
            node:"4",
            state:"on"
          },
          success(res){
            console.log("设备打开")
          }
        })
      }
      // 判断switch是打开还是关闭
      else if(sw == false){
        wx.request({
          url: 'http://192.168.43.53/handle',
          header:{
           'content-type':'application/x-www-form-urlencoded',  
           'Connection':'keep-alive',
           'Accept-Language':'zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6',
          },
          method :"POST",
          data:{
            node:"4",
            state:"off"
          },
          // 请求成功后的回调
          success(res){
            console.log("设备关闭")
          }
        })
      }
    },

})


