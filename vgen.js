#!/usr/bin/env node
const program = require('/usr/lib/node_modules/commander');
const StellarSdk = require('/usr/lib/node_modules/stellar-sdk');
const csv = require('/usr/lib/node_modules/fast-csv')
const mongo = require('/usr/lib/node_modules/mongodb').MongoClient;
const assert = require('assert')

program
  .version('0.1')
  .option('-p, --prefix', 'Find address that begins with the words specified')
  .option('-s, --suffix', 'Find address that end with the words specified')
  .parse(process.argv);
suffix = program.suffix || (!program.prefix && !program.suffix);


const url = "mongodb://" + (process.env.MONGO_PASS != null ? process.env.MONGO_PASS + "@" : "") + "localhost:27017";
const nBench = 1000;


VanitySearch = function(words, suffix) {

  console.log(`Finding address ...`);

  mongo.connect(url, { useNewUrlParser: true }, async function(err, db) {
    if (err) throw err;

    var dbo = db.db("mydb");

    var i = oldHit = newHit = 0;
    var oldTime = newTime = process.hrtime();
    do {
      const pair = StellarSdk.Keypair.random();
      const publicKey = pair.publicKey();
      i = i + 1;
      if (i % nBench == 0) {
        newTime = process.hrtime();
        dt = (newTime[0] - oldTime[0]) + (newTime[1] - oldTime[1])*1e-9
        console.error('Generation rate:', Math.round(1.0 * nBench / dt), 'addr/sec\t',
                      'Hit rate: ', Math.round(100*(newHit - oldHit)/dt)/100, 'addr/sec');
        oldTime = newTime;
        oldHit = newHit;
      }
      var word = suffix ? words.find(w => publicKey.endsWith(w)) : words.find(w => publicKey.startsWith(w, 1));
      if (word != null) {
        let r = await dbo.collection("StellarVanityPool").insertOne({ "suffix": word, "public": pair.publicKey(), "secret": pair.secret() });
        assert.equal(1, r.insertedCount);
        newHit = newHit + 1;
      }
    } while (true);
  });
};


console.log(`Reading names...`);
var names = new Array();
csv.fromPath("./names/NAMES.txt", {headers: false, ltrim: true, rtrim: true})
  .on("data", function(data) {
    names.push(data[0]);
  })
  .on("end", function() {
    console.log(`Done.`);

    const words = names.filter((v,i,a) => { return v.length > 2 });

    console.log('Loaded', words.length, 'suffixes.');
    VanitySearch(words, suffix);
  });
