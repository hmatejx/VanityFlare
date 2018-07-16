# clean-up
#rm(list = ls())

library(mongolite)

# connect to MongoDB
if (!exists("db") || db$info()$server$ok != 1) {
  if (exists("db")) 
    rm(db)
  db <- mongo("StellarVanityPool", "mydb", "mongodb://admin:aaaa1111@localhost")
}

# basic stats
cat("Total number of generated addresses:", db$count("{}"), "\n")


# get counts
lens <- 3:8
gens <- sapply(lens, function(i) {
  db$count(paste('{"$expr": { "$eq": [ { "$strLenCP": "$suffix" },', i, '] }}'))
})

# plot results
plot(lens, gens, log = "y", type = "b", ylim = c(1, 100000000), xlab = "Length", ylab = "Count")
text(lens, gens, labels = gens, adj = c(-0.25, -0.5))
abline(h = 10^(0:8), lwd = 0.5, lty = 1, col = "gray")
abline(v = lens, lwd = 0.5, lty = 1, col = "gray")
title(main = "Generated Stellar Vanity Addresses")
box()

# print interesting suffixes
cat("Interesting addresses:\n")
print(sort(db$find('{"$expr": { "$gt": [ { "$strLenCP": "$suffix" }, 6 ] }}')$suffix))

# close connection
#rm(db)