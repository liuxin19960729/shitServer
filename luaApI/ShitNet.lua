---@diagnostic disable: missing-return
ShitNet = {}
---创建新服务
---@param nameStr string 服务名
---@return integer 服务ID
function ShitNet.NewService(nameStr) end

---kill 服务
---@param id integer 服务id
function ShitNet.Killservice(id) end

---发送信息
---@param sourceId integer 发送人id
---@param toId integer  接收人id
---@param strMsg string  发送字符串信息
function ShitNet.Send(sourceId, toId, strMsg) end
