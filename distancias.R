
N = 60
distancias = round(as.matrix(dist(runif(N,0,round(N*N)))),2)
trace = matrix(0,ncol=N,nrow=N)
write.table(distancias,
          'distances.csv',
          col.names=F,sep=',',
          row.names=F)

write.table(trace,
          'trace.csv',
          col.names=F,sep=',',
          row.names=F)
