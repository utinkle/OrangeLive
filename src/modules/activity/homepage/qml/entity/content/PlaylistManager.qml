pragma Singleton
import QtQuick
import Qt.labs.settings

QtObject {
    id: root

    // ---------- 数据结构 ----------
    // 播放列表源: { name: string, url: string, isDefault: bool, channels: ListModel }
    property var sources: []
    property int currentSourceIndex: -1

    // 频道模型（当前选中源的频道列表，经过过滤）
    property ListModel filteredChannelsModel: ListModel {}
    property string currentGroup: ""
    property string searchKeyword: ""

    // 收藏夹: 存储 url -> 标题等，但为了简单，存储 url 列表
    property var favorites: []
    // 历史记录: 存储 { url, title, timestamp }
    property var history: []

    // 加载状态
    property bool loading: false
    property string errorString: ""

    // 配置存储
    Settings {
        id: settings
        category: "PlaylistManager"
    }

    function loadSettings() {
        var savedSources = settings.value("sources", [])
        if (savedSources.length === 0) {
            // 默认添加一个示例源（可替换为你的默认源）
            addSource("默认列表", "", true)
        } else {
            // 恢复源
            for (var i = 0; i < savedSources.length; ++i) {
                var src = savedSources[i]
                var channelsModel = ListModel {}
                // 恢复频道列表（如果保存过）
                var savedChannels = settings.value("channels_" + src.name, [])
                for (var j = 0; j < savedChannels.length; ++j) {
                    channelsModel.append(savedChannels[j])
                }
                sources.push({
                    name: src.name,
                    url: src.url,
                    isDefault: src.isDefault,
                    channels: channelsModel
                })
                if (src.isDefault) currentSourceIndex = i
            }
            if (currentSourceIndex === -1 && sources.length > 0) currentSourceIndex = 0
        }

        // 加载收藏和历史
        favorites = settings.value("favorites", [])
        history = settings.value("history", [])
    }

    function saveSettings() {
        var saveSources = []
        for (var i = 0; i < sources.length; ++i) {
            var src = sources[i]
            saveSources.push({
                name: src.name,
                url: src.url,
                isDefault: src.isDefault
            })
            // 保存频道列表
            var channelsArray = []
            for (var j = 0; j < src.channels.count; ++j) {
                var ch = src.channels.get(j)
                channelsArray.push({
                    name: ch.name,
                    title: ch.title,
                    url: ch.url,
                    group: ch.group,
                    isLive: ch.isLive,
                    tvgId: ch.tvgId,
                    tvgName: ch.tvgName,
                    tvgLogo: ch.tvgLogo
                })
            }
            settings.setValue("channels_" + src.name, channelsArray)
        }
        settings.setValue("sources", saveSources)
        settings.setValue("favorites", favorites)
        settings.setValue("history", history)
    }

    // 添加播放列表源
    function addSource(name, url, isDefault) {
        var newSource = {
            name: name,
            url: url,
            isDefault: isDefault,
            channels: ListModel {}
        }
        sources.push(newSource)
        if (isDefault) currentSourceIndex = sources.length - 1
        saveSettings()
        if (currentSourceIndex === sources.length - 1 && url !== "") {
            loadSourceChannels(sources.length - 1)
        }
    }

    // 删除源
    function removeSource(index) {
        if (index < 0 || index >= sources.length) return
        if (sources[index].isDefault && sources.length > 1) {
            // 不能删除默认源，可以设置另一个为默认
            for (var i = 0; i < sources.length; ++i) {
                if (i !== index) {
                    sources[i].isDefault = true
                    currentSourceIndex = i
                    break
                }
            }
        }
        sources.splice(index, 1)
        if (currentSourceIndex >= sources.length) currentSourceIndex = sources.length - 1
        if (currentSourceIndex >= 0) {
            loadSourceChannels(currentSourceIndex)
        } else {
            filteredChannelsModel.clear()
        }
        saveSettings()
    }

    // 加载指定源的频道列表（m3u解析）
    function loadSourceChannels(sourceIdx) {
        if (sourceIdx < 0 || sourceIdx >= sources.length) return
        var src = sources[sourceIdx]
        if (!src.url || src.url === "") {
            // 空源，清空模型
            src.channels.clear()
            applyFilter()
            return
        }
        loading = true
        errorString = ""
        // 使用 XmlHttpRequest 或 fetch 加载 m3u
        var xhr = new XMLHttpRequest()
        xhr.open("GET", src.url)
        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                loading = false
                if (xhr.status === 200) {
                    parseM3u(xhr.responseText, src)
                } else {
                    errorString = "加载失败: " + xhr.statusText
                }
                applyFilter()
                saveSettings()  // 保存频道列表到设置
            }
        }
        xhr.send()
    }

    // 解析 m3u 内容
    function parseM3u(content, sourceObj) {
        var lines = content.split(/\r?\n/)
        var channels = []
        var currentItem = {}
        for (var i = 0; i < lines.length; i++) {
            var line = lines[i].trim()
            if (line.startsWith("#EXTINF:")) {
                // 解析属性
                var match = line.match(/tvg-id="([^"]*)"/)
                var tvgId = match ? match[1] : ""
                match = line.match(/tvg-name="([^"]*)"/)
                var tvgName = match ? match[1] : ""
                match = line.match(/tvg-logo="([^"]*)"/)
                var tvgLogo = match ? match[1] : ""
                match = line.match(/group-title="([^"]*)"/)
                var group = match ? match[1] : "未分组"
                // 频道名称在逗号后面
                var namePart = line.split(",")
                var title = namePart.length > 1 ? namePart.slice(1).join(",").trim() : ""
                if (title === "") title = tvgName || tvgId || "未知频道"
                currentItem = {
                    name: title,
                    title: title,
                    group: group,
                    tvgId: tvgId,
                    tvgName: tvgName,
                    tvgLogo: tvgLogo,
                    isLive: true,  // 默认为直播，后续可根据 URL 判断
                    url: ""
                }
            } else if (line && !line.startsWith("#") && currentItem.url === "") {
                currentItem.url = line
                // 判断是否为直播流
                if (line.toLowerCase().endsWith(".m3u8") || line.toLowerCase().endsWith(".flv") || line.startsWith("rtsp")) {
                    currentItem.isLive = true
                } else {
                    currentItem.isLive = false
                }
                channels.push(currentItem)
                currentItem = {}
            }
        }
        // 更新 source 的 channels 模型
        sourceObj.channels.clear()
        for (var j = 0; j < channels.length; j++) {
            sourceObj.channels.append(channels[j])
        }
    }

    // 切换当前源
    function setCurrentSource(index) {
        if (index === currentSourceIndex) return
        currentSourceIndex = index
        if (currentSourceIndex >= 0) {
            loadSourceChannels(currentSourceIndex)
        } else {
            filteredChannelsModel.clear()
        }
        // 重置过滤条件
        currentGroup = ""
        searchKeyword = ""
    }

    // 应用分组和搜索过滤
    function applyFilter() {
        if (currentSourceIndex < 0) return
        var src = sources[currentSourceIndex]
        var allChannels = []
        for (var i = 0; i < src.channels.count; ++i) {
            allChannels.push(src.channels.get(i))
        }
        var filtered = allChannels
        if (currentGroup !== "" && currentGroup !== "全部") {
            filtered = filtered.filter(ch => ch.group === currentGroup)
        }
        if (searchKeyword !== "") {
            var kw = searchKeyword.toLowerCase()
            filtered = filtered.filter(ch => ch.title.toLowerCase().includes(kw) || ch.name.toLowerCase().includes(kw))
        }
        // 更新 filteredChannelsModel
        filteredChannelsModel.clear()
        for (var j = 0; j < filtered.length; ++j) {
            filteredChannelsModel.append(filtered[j])
        }
    }

    // 获取所有分组名称
    function getGroups() {
        if (currentSourceIndex < 0) return []
        var src = sources[currentSourceIndex]
        var groupsSet = {}
        for (var i = 0; i < src.channels.count; ++i) {
            var grp = src.channels.get(i).group
            if (grp) groupsSet[grp] = true
        }
        var groups = Object.keys(groupsSet)
        groups.sort()
        return groups
    }

    // 收藏操作
    function isFavorite(url) {
        return favorites.indexOf(url) !== -1
    }
    function addFavorite(url) {
        if (!isFavorite(url)) {
            favorites.push(url)
            saveSettings()
        }
    }
    function removeFavorite(url) {
        var idx = favorites.indexOf(url)
        if (idx !== -1) {
            favorites.splice(idx, 1)
            saveSettings()
        }
    }
    function toggleFavorite(url) {
        if (isFavorite(url)) removeFavorite(url)
        else addFavorite(url)
    }

    // 历史记录
    function addHistory(channel) {
        // 避免重复添加同一频道连续多次，简单去重：移除旧的相同url再插入到头部
        var idx = -1
        for (var i = 0; i < history.length; ++i) {
            if (history[i].url === channel.url) {
                idx = i
                break
            }
        }
        if (idx !== -1) history.splice(idx, 1)
        history.unshift({
            url: channel.url,
            title: channel.title,
            group: channel.group,
            timestamp: new Date().getTime()
        })
        // 限制最多100条
        while (history.length > 100)
            history.pop()

        saveSettings()
    }

    // 初始化
    Component.onCompleted: {
        loadSettings()
        // 如果还没有加载任何源，尝试加载默认源频道
        if (currentSourceIndex >= 0 && sources[currentSourceIndex].url !== "") {
            loadSourceChannels(currentSourceIndex)
        }
    }
}