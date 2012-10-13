library("ggplot2")

# Histogram plots of VTS / visibile days
sockets <- read.csv("results-correct/ServerSocketStability_overall_external.csv", sep=",")
#sockets <- read.csv("results-old/ServerSocketStability_overall_external.csv", sep=",")
#p <- ggplot(sockets, aes(VisibleTimeSlots)) + geom_bar() + facet_wrap( ~ VisibleDays, scales="free")
#ggsave(file="VTS_by_visibledays.pdf", width=12, height=10, dpi=300)

sockets$Ratio <- sockets$BidirectionalFlows/(sockets$BidirectionalFlows+sockets$UnidirectionalFlowsOut)
#sockets$Ratio <- sockets$AverageRatio



# Ratio ECDF plots
ecdf_ratio_10VTS <- ecdf(subset(sockets$Ratio, sockets$VisibleTimeSlots >=10))
ecdf_ratio_100VTS <- ecdf(subset(sockets$Ratio, sockets$VisibleTimeSlots >=100))
ecdf_ratio_1000VTS <- ecdf(subset(sockets$Ratio, sockets$VisibleTimeSlots >=1000))
ecdf_ratio_1200VTS <- ecdf(subset(sockets$Ratio, sockets$VisibleTimeSlots >=1200))
ecdf_ratio_Days1 <- ecdf(subset(sockets$Ratio, sockets$VisibleDays == 1))
ecdf_ratio_Days2 <- ecdf(subset(sockets$Ratio, sockets$VisibleDays == 2))
ecdf_ratio_Days3 <- ecdf(subset(sockets$Ratio, sockets$VisibleDays == 3))
ecdf_ratio_Days4 <- ecdf(subset(sockets$Ratio, sockets$VisibleDays == 4))
ecdf_ratio_Days5 <- ecdf(subset(sockets$Ratio, sockets$VisibleDays == 5))
ecdf_ratio_Days6 <- ecdf(subset(sockets$Ratio, sockets$VisibleDays == 6))

x <- seq(from=0, to=1, by=0.001)
ecdf_df1 <- data.frame(   x=x,
                    ecdf_vts = c(ecdf_ratio_10VTS(x), ecdf_ratio_100VTS(x), ecdf_ratio_1000VTS(x), ecdf_ratio_1200VTS(x)),
                    VisibleTimeSlots = rep(c(">10", ">100", ">1000", ">1200"), each=length(x))
                )
ecdf_df2 <- data.frame(  x=x,
                    ecdf_days = c(ecdf_ratio_Days1(x), ecdf_ratio_Days2(x), ecdf_ratio_Days3(x), ecdf_ratio_Days4(x), ecdf_ratio_Days5(x), ecdf_ratio_Days6(x)),
                    VisibleDays = rep(c("1 day", "2 days", "3 days", "4 days", "5 days", "6 days"), each=length(x))
                )

p <- qplot(data=ecdf_df1, x, 1-ecdf_vts, geom="step", colour = VisibleTimeSlots, xlab="Stability", ylab="CCDF", xlim=range(0,1), direction= "hv") + scale_color_hue(name="Visible Time Slots")
ggsave(file="CCDF_ratio_VTS.pdf", width=12, height=8, dpi=300)

q <- qplot(data=ecdf_df2, x, 1-ecdf_days, geom="step", colour = VisibleDays, xlab="Stability", ylab="CCDF", xlim=range(0,1), direction= "hv") + scale_color_hue(name="Visible Days")
ggsave(file="CCDF_ratio_days.pdf", width=12, height=8, dpi=300)

d <- ggplot(subset(sockets, sockets$Port ==80), aes(Ratio, BidirectionalFlows+UnidirectionalFlowsOut))
d + geom_point(alpha=0.2) + geom_point(data=subset(sockets, sockets$Port ==80), colour ="red", alpha=0.2) + geom_point(data=subset(sockets, sockets$Port == 53), color ="Blue", alpha=0.2)

# aggregate the socket by port and sum their flows
tmp_sockets <-  data.frame(
                    IP=sockets$X..IP, Port=sockets$Port, Protocol=sockets$Protocol,
                    Flows=as.numeric( sockets$BidirectionalFlows+sockets$UnidirectionalFlowsIn+sockets$UnidirectionalFlowsOut)
                )
port_max <- aggregate(data=tmp_sockets, Flows ~ Port + Protocol, sum)
port_max_length <- aggregate(data=tmp_sockets, Flows ~ Port + Protocol, length)
port_max$Flows_percent <- port_max$Flows/sum(port_max$Flows)
port_max$Sockets <- port_max_length$Flows
port_max$Sockets_percent <- port_max$Sockets/sum(port_max$Sockets)
port_max <- port_max[order(port_max$Flows, decreasing=TRUE),]
top_20 <- port_max[1:20,]
other_ports <- port_max[21:nrow(port_max),]
high_ports <- subset(other_ports, Port > 1023 )
high_ports_by_P <- merge(
                            aggregate(data=high_ports, Flows ~ Protocol, sum),
                            aggregate(data=high_ports, Sockets ~ Protocol, sum)
                        )
high_ports_by_P$Port <- rep("high", times=nrow(high_ports_by_P))
high_ports_by_P$Flows_percent <- high_ports_by_P$Flows/sum(port_max$Flows)
high_ports_by_P$Sockets_percent <- high_ports_by_P$Sockets/sum(port_max$Sockets)
low_ports <- subset(other_ports, Port <= 1023 )
low_ports_by_P <- merge(
                            aggregate(data=low_ports, Flows ~ Protocol, sum),
                            aggregate(data=low_ports, Sockets ~ Protocol, sum)
                        )
low_ports_by_P$Port <- rep("low", times=nrow(low_ports_by_P))
low_ports_by_P$Flows_percent <- low_ports_by_P$Flows/sum(port_max$Flows)
low_ports_by_P$Sockets_percent <- low_ports_by_P$Sockets/sum(port_max$Sockets)

# Plot the Stability of the top 20 ports
tmp_sockets1 <- subset(sockets, Port %in% sort(top_20$Port))
tmp_sockets1$Port <- as.character(tmp_sockets1$Port)
tmp_sockets2 <- subset(sockets, !(Port %in% sort(top_20$Port)))
tmp_sockets2$Port <- rep("other",times=nrow(tmp_sockets2))
tmp_sockets <- rbind(tmp_sockets1, tmp_sockets2)
p <- ggplot(tmp_sockets, aes(Port, Ratio), xlab="Port")
p + geom_boxplot(outlier.size = 1, size=0.25, aes(fill=factor(Protocol))) + scale_fill_hue(name="Protocol", labels=c("TCP", "UDP")) + xlab("Port") + ylab("Stability")
ggsave(file="top20_ratio_box.pdf", width=12, height=8, dpi=300)

# Plot the Visibility of the top 20 ports
p <- ggplot(tmp_sockets, aes(Port, VisibleDays))
p + geom_boxplot(outlier.size = 1, size =0.25, aes(fill=factor(Protocol))) + scale_fill_hue(name="Protocol", labels=c("TCP", "UDP")) + xlab("Port") + ylab("Visibility")
ggsave(file="top20_visibility_box.pdf", width=12, height=8, dpi=300)

# PLOT THE HEAT MAP FOR SHOWING THE DIMENSIONS
sockets$TotalFlows <- sockets$BidirectionalFlows + sockets$UnidirectionalFlowsIn
ecdf_pop <- ecdf(sockets$TotalFlows)
sockets$PopularityRank <- ecdf_pop(sockets$TotalFlows)
ecdf_vis <- ecdf(sockets$VisibleTimeSlots)
sockets$VisibilityRank <- ecdf_vis(sockets$VisibleTimeSlots)

# STABILITY VISIBILIY MAP
p <- ggplot(sockets, aes(x=AverageRatio, y=VisibilityRank))
p + geom_bin2d(binwidth = c(0.001,0.001)) + xlab("Stability") + ylab("Visibility Quantiles") + guides(fill=FALSE)
ggsave(file="visibiliy_stability_map.pdf", width=12, height=8, dpi=300)

# CREATE THE POPULARITY MAP
p <- ggplot(sockets, aes(x=AverageRatio, y=PopularityRank))
p + geom_bin2d(binwidth = c(0.001,0.001)) + xlab("Stability") + ylab("Popularity Quantiles") + guides(fill=FALSE)
ggsave(file="popularity_stability_map.pdf", width=12, height=8, dpi=300)
