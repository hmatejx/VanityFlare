# clean-up
#rm(list = ls())

library(mongolite)

# connect to MongoDB
if (!exists("db") || db$info()$server$ok != 1) {
  if (exists("db")) 
    rm(db)
  url <- "mongodb://"
  if (nchar(Sys.getenv("MONGO_PASS")) > 0) 
    url <- paste0(url, Sys.getenv("MONGO_PASS"), "@localhost:27017")
  else
    url <- paste0(url, "localhost:27017")
  db <- mongo("StellarVanityPool", "mydb", url)
}

# basic stats
tot <- db$count("{}")
cat("Total number of generated addresses:", tot, "\n")


# get counts
lens <- 3:10
gens <- sapply(lens, function(i) {
  db$count(paste('{"len": { "$eq":', i , '}}'))
})

# plot results
plot(lens, gens, log = "y", type = "b", ylim = c(1, 1e10), xlab = "Length", ylab = "Count")
text(lens, gens, labels = gens, adj = c(-0.25, -0.5))
abline(h = 10^(0:9), lwd = 0.5, lty = 1, col = "gray")
abline(v = lens, lwd = 0.5, lty = 1, col = "gray")
title(main = paste0("Generated Stellar Vanity Addresses (", tot, " in total)"))
box()

# print interesting suffixes
cat("Interesting addresses:\n")
int <-unique(sort(db$find('{"len": { "$gt": 7 }}')$suffix))
print(int)

# close connection
#rm(db)
