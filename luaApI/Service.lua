---服务Init事件被触发
---@param id any 服务ID
function OnInit(id) end

---服务退出前触发的事件
function OnExit() end

---接收另一个服务发送的消息
---@param source integer 发送者id
---@param buff string 接收的字符串信息
function OnServiceMsg(source, buff) end
