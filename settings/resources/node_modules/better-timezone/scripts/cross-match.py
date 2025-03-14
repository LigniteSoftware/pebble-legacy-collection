import json

with open('moment-iana-abbr.json') as json_data1:
    with open('name-to-abbr.json') as json_data2:
        moment_iana_abbr = json.load(json_data1)
        name_to_abbr = json.load(json_data2)
        new_name_iana = []

        def find_name(iana):
            name = 'NOPE'
            for x in name_to_abbr:
                if iana in x["mapZone"]["_type"].split(' '):
                    name = x["mapZone"]["_other"]
            return name

        for i, obj in enumerate(moment_iana_abbr):
            print str(len(moment_iana_abbr) - i) + " entries left"
            iana = obj["name"]
            abbrs = obj["abbrs"]
            name = find_name(iana)
            new_name_iana.append({
                "iana": iana,
                "abbrs": abbrs,
                "name": name
                })

            print "Done, writing to file"
            with open('crossed.json', 'w') as outfile:
                json.dump(new_name_iana, outfile, indent=2)
