#!/usr/bin/rinterp

library("ggplot2")
data <- read.csv("traffic_stats_port80.csv", sep=',')

flow_stats <- rbind(
    data.frame(Type="Flows bi monitored", time=data$Time, Flows=data$FlowsBiMonitored),
    data.frame(Type="Flows bi not monitored", time=data$Time, Flows=data$FlowsBiNotMonitored),
    data.frame(Type="Flows uni out monitored", time=data$Time, Flows=data$FlowsUniOutMonitored),
    data.frame(Type="Flows uni out not monitored", time=data$Time, Flows=data$FlowsUniOutNotMonitored),
    data.frame(Type="Flows uni in monitored", time=data$Time, Flows=data$FlowsUniInMonitored),
    data.frame(Type="Flows uni in not monitored", time=data$Time, Flows=data$FlowsUniInNotMonitored)
)
# add a human readable date for the plot
flow_stats$Date <- as.POSIXct(flow_stats$time, origin="1970-1-1")

#create the flow by Type plot
p <- ggplot(flow_stats, aes(Date, Flows, fill=Type)) + geom_area()
# For labeling the axis use scale_y_continuous(name="Flows by Type")
ggsave(file="Flows_by_type_area.pdf",width=12, height=8,dpi=300)
p <- ggplot(flow_stats, aes(Date, Flows, color=Type)) + geom_line()
ggsave(file="Flows_by_type_lines.pdf",width=12, height=8,dpi=300)

# create the ratio plots
ratios <- rbind(
    data.frame(direction="Bidirectional", Type="Flows Monitored", time=data$Time, Ratio=(data$FlowsBiMonitored/(data$FlowsBiMonitored+data$FlowsBiNotMonitored))),
    data.frame(direction="Bidirectional", Type="Flows Not Monitored", time=data$Time, Ratio=data$FlowsBiNotMonitored/(data$FlowsBiMonitored+data$FlowsBiNotMonitored)),
    data.frame(direction="Unidirectional Out", Type="Flows Monitored", time=data$Time, Ratio=data$FlowsUniOutMonitored/(data$FlowsUniOutMonitored+data$FlowsUniOutNotMonitored)),
    data.frame(direction="Unidirectional Out", Type="Flows Not Monitored", time=data$Time, Ratio=data$FlowsUniOutNotMonitored/(data$FlowsUniOutMonitored+data$FlowsUniOutNotMonitored)),
    data.frame(direction="Unidirectioanl In", Type="Flows Monitored", time=data$Time, Ratio=data$FlowsUniInMonitored/(data$FlowsUniInMonitored+data$FlowsUniInNotMonitored)),
    data.frame(direction="Unidirectioanl In", Type="Flows Not Monitored", time=data$Time, Ratio=data$FlowsUniInNotMonitored/(data$FlowsUniInMonitored+data$FlowsUniInNotMonitored))
)
ratios$Date <- as.POSIXct(ratios$time, origin="1970-1-1")

p <- ggplot(ratios, aes(Date, Ratio, fill=Type)) + geom_area(position = 'stack') + facet_grid(direction ~ ., scales="free_y") + ylim(0,1)
ggsave(file="Flows_monitor_ratio_by_type.pdf",width=12, height=10, dpi=300)

# Port Heuristical Plots of unmonitored Flows
ports <- rbind(
    data.frame(direction="Bidirectional", Type="Out High, In High", time=data$Time, Flows=data$FlowsBiNMOutHighInHigh),
    data.frame(direction="Bidirectional", Type="Out High, In Low", time=data$Time, Flows=data$FlowsBiNMOutHighInLow),
    data.frame(direction="Bidirectional", Type="Out Low, In Low", time=data$Time, Flows=data$FlowsBiNMOutLowInLow),
    data.frame(direction="Bidirectional", Type="Out Low, In High", time=data$Time, Flows=data$FlowsBiNMOutLowInHigh),
    data.frame(direction="Unidirectional Out", Type="Out High, In High", time=data$Time, Flows=data$FlowsUniOutNMOutHighInHigh),
    data.frame(direction="Unidirectional Out", Type="Out High, In Low", time=data$Time, Flows=data$FlowsUniOutNMOutHighInLow),
    data.frame(direction="Unidirectional Out", Type="Out Low, In Low", time=data$Time, Flows=data$FlowsUniOutNMOutLowInLow),
    data.frame(direction="Unidirectional Out", Type="Out Low, In High", time=data$Time, Flows=data$FlowsUniOutNMOutLowInHigh),
    data.frame(direction="Unidirectional In", Type="Out High, In High", time=data$Time, Flows=data$FlowsUniInNMOutHighInHigh),
    data.frame(direction="Unidirectional In", Type="Out High, In Low", time=data$Time, Flows=data$FlowsUniInNMOutHighInLow),
    data.frame(direction="Unidirectional In", Type="Out Low, In Low", time=data$Time, Flows=data$FlowsUniInNMOutLowInLow),
    data.frame(direction="Unidirectional In", Type="Out Low, In High", time=data$Time, Flows=data$FlowsUniInNMOutLowInHigh)
)
ports$Date <- as.POSIXct(ports$time, origin="1970-1-1")
p <- ggplot(ports, aes(Date, Flows, fill=Type)) + geom_area(position = 'stack') + facet_grid(direction ~ ., scales="free_y")
ggsave(file="Flows_unmonitored_by_port.pdf", width=12, height=10, dpi=300)


# and the same stuff for Bytes

flow_stats <- rbind(
    data.frame(Type="Bytes bi monitored", time=data$Time, Bytes=data$BytesBiMonitored),
    data.frame(Type="Bytes bi not monitored", time=data$Time, Bytes=data$BytesBiNotMonitored),
    data.frame(Type="Bytes uni out monitored", time=data$Time, Bytes=data$BytesUniOutMonitored),
    data.frame(Type="Bytes uni out not monitored", time=data$Time, Bytes=data$BytesUniOutNotMonitored),
    data.frame(Type="Bytes uni in monitored", time=data$Time, Bytes=data$BytesUniInMonitored),
    data.frame(Type="Bytes uni in not monitored", time=data$Time, Bytes=data$BytesUniInNotMonitored)
)
# add a human readable date for the plot
flow_stats$Date <- as.POSIXct(flow_stats$time, origin="1970-1-1")

#create the flow by Type plot
p <- ggplot(flow_stats, aes(Date, Bytes, fill=Type)) + geom_area()
ggsave(file="Bytes_by_type_area.pdf",width=12, height=8,dpi=300)
p <- ggplot(flow_stats, aes(Date, Bytes, color=Type)) + geom_line()
ggsave(file="Bytes_by_type_lines.pdf",width=12, height=8,dpi=300)

# create the ratio plots
ratios <- rbind(
    data.frame(direction="Bidirectional", Type="Bytes Monitored", time=data$Time, Ratio=(data$BytesBiMonitored/(data$BytesBiMonitored+data$BytesBiNotMonitored))),
    data.frame(direction="Bidirectional", Type="Bytes Not Monitored", time=data$Time, Ratio=data$BytesBiNotMonitored/(data$BytesBiMonitored+data$BytesBiNotMonitored)),
    data.frame(direction="Unidirectional Out", Type="Bytes Monitored", time=data$Time, Ratio=data$BytesUniOutMonitored/(data$BytesUniOutMonitored+data$BytesUniOutNotMonitored)),
    data.frame(direction="Unidirectional Out", Type="Bytes Not Monitored", time=data$Time, Ratio=data$BytesUniOutNotMonitored/(data$BytesUniOutMonitored+data$BytesUniOutNotMonitored)),
    data.frame(direction="Unidirectioanl In", Type="Bytes Monitored", time=data$Time, Ratio=data$BytesUniInMonitored/(data$BytesUniInMonitored+data$BytesUniInNotMonitored)),
    data.frame(direction="Unidirectioanl In", Type="Bytes Not Monitored", time=data$Time, Ratio=data$BytesUniInNotMonitored/(data$BytesUniInMonitored+data$BytesUniInNotMonitored))
)
ratios$Date <- as.POSIXct(ratios$time, origin="1970-1-1")

p <- ggplot(ratios, aes(Date, Ratio, fill=Type)) + geom_area(position = 'stack') + facet_grid(direction ~ ., scales="free_y") + ylim(0,1)
ggsave(file="Bytes_monitor_ratio_by_type.pdf",width=12, height=10, dpi=300)

# Port Heuristical Plots of unmonitored Bytes
ports <- rbind(
    data.frame(direction="Bidirectional", Type="Out High, In High", time=data$Time, Bytes=data$BytesBiNMOutHighInHigh),
    data.frame(direction="Bidirectional", Type="Out High, In Low", time=data$Time, Bytes=data$BytesBiNMOutHighInLow),
    data.frame(direction="Bidirectional", Type="Out Low, In Low", time=data$Time, Bytes=data$BytesBiNMOutLowInLow),
    data.frame(direction="Bidirectional", Type="Out Low, In High", time=data$Time, Bytes=data$BytesBiNMOutLowInHigh),
    data.frame(direction="Unidirectional Out", Type="Out High, In High", time=data$Time, Bytes=data$BytesUniOutNMOutHighInHigh),
    data.frame(direction="Unidirectional Out", Type="Out High, In Low", time=data$Time, Bytes=data$BytesUniOutNMOutHighInLow),
    data.frame(direction="Unidirectional Out", Type="Out Low, In Low", time=data$Time, Bytes=data$BytesUniOutNMOutLowInLow),
    data.frame(direction="Unidirectional Out", Type="Out Low, In High", time=data$Time, Bytes=data$BytesUniOutNMOutLowInHigh),
    data.frame(direction="Unidirectional In", Type="Out High, In High", time=data$Time, Bytes=data$BytesUniInNMOutHighInHigh),
    data.frame(direction="Unidirectional In", Type="Out High, In Low", time=data$Time, Bytes=data$BytesUniInNMOutHighInLow),
    data.frame(direction="Unidirectional In", Type="Out Low, In Low", time=data$Time, Bytes=data$BytesUniInNMOutLowInLow),
    data.frame(direction="Unidirectional In", Type="Out Low, In High", time=data$Time, Bytes=data$BytesUniInNMOutLowInHigh)
)
ports$Date <- as.POSIXct(ports$time, origin="1970-1-1")
p <- ggplot(ports, aes(Date, Bytes, fill=Type)) + geom_area(position = 'stack') + facet_grid(direction ~ ., scales="free_y")
ggsave(file="Bytes_unmonitored_by_port.pdf", width=12, height=10, dpi=300)
