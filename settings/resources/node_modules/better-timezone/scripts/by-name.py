import json

with open('../data/final.json') as json_data:
    final = json.load(json_data)
    by_name = {}

    for i, obj in enumerate(final):
        print str(len(final) - i) + " entries left"
        if obj["name"] not in by_name:
            by_name[obj["name"]] = [{
                "iana": obj["iana"],
                "abbrs": obj["abbrs"]
            }]
        else:
            by_name[obj["name"]].append({
                "iana": obj["iana"],
                "abbrs": obj["abbrs"]
            })

        print "Done, writing to file"
        with open('../data/by_name.json', 'w') as outfile:
            json.dump(by_name, outfile, indent=2)
