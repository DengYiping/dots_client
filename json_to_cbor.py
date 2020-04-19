import cbor
import json
from argparse import ArgumentParser

JSON_TO_CBOR_MAPPER = {
    'ietf-dots-signal-channel:mitigation-scope': 1,
    'scope': 2,
    'cdid': 3,
    'cuid': 4,
    'mid': 5,
    'target-prefix': 6,
    'target-port-range': 7,
    'lower-port': 8,
    'upper-port': 9,
    'target-protocol': 10,
    'target-fqdn': 11,
    'target-uri': 12,
    'alias-name': 13,
    'lifetime': 14,
    'mitigation-start': 15,
    'status': 16,
    'conflict-information': 17,
    'conflict-status': 18,
    'conflict-cause': 19,
    'retry-timer': 20,
    'conflict-scope': 21,
    'acl-list': 22,
    'acl-name': 23,
    'acl-type': 24,
    'bytes-dropped': 25,
    'bps-dropped': 26,
    'pkts-dropped': 27,
    'pps-dropped': 28,
    'attack-status': 29,
    'ietf-dots-signal-channel:signal-config': 30,
    'sid': 31,
    'mitigating-config': 32,
    'heartbeat-interval': 33,
    'max-value': 34,
    'min-value': 35,
    'current-value': 36,
    'missing-hb-allowed': 37,
    'max-retransmit': 38,
    'ack-timeout': 39,
    'ack-random-factor': 40,
    'max-value-decimal': 41,
    'min-value-decimal': 42,
    'current-value-decimal': 43,
    'idle-config': 44,
    'trigger-mitigation': 45,
    'ietf-dots-signal-channel:redirected-signal': 46,
    'alt-server': 47,
    'alt-server-record': 48,
    'ietf-dots-signal-channel:heartbeat': 49,
    'probing-rate': 50,
    'peer-hb-status': 51
}

CBOR_TO_JSON_MAPPER = {v:k for k, v in JSON_TO_CBOR_MAPPER.items()}

def main():
    parser = ArgumentParser(description='Convert DOTS json payload to DOTS cbor payload')
    parser.add_argument('input_file')
    parser.add_argument('output_file')
    parser.add_argument('--to_json', dest='convert', action='store_const', const=to_json, default=to_cbor, help='Convert cbor to json representation')
    args = parser.parse_args()
    input_file, output_file = args.input_file, args.output_file
    conversion = args.convert
    conversion(input_file, output_file)

def to_cbor(json_file, output_file):
    print('Converting to cbor')
    with open(json_file, 'r') as fi:
        with open(output_file, 'wb') as fo:
            dic = json.load(fi)
            dic_mapped = replace_dict_key_recursively(dic, JSON_TO_CBOR_MAPPER)
            cbor.dump(dic_mapped, fo)

def to_json(cbor_file, output_file):
    print('Converting to json')
    with open(cbor_file, 'rb') as fi:
        with open(output_file, 'w') as fo:
            dic = cbor.load(fi)
            dic_mapped = replace_dict_key_recursively(dic, CBOR_TO_JSON_MAPPER)
            json.dump(dic_mapped, fo)

"""
Replace all the key in a dictionary recursively.
"""
def replace_dict_key_recursively(root_dict, key_lookup):
    if isinstance(root_dict, dict):
        result = {}
        for k, v in root_dict.items():
            if k in key_lookup:
                mapper = key_lookup[k]
                if isinstance(mapper, tuple):
                    cbor_key, value_mapper = mapper
                    result[cbor_key] = value_mapper(v)
                else:
                    result[mapper] = replace_dict_key_recursively(v, key_lookup)
            else:
                result[k] = replace_dict_key_recursively(v, key_lookup)
        return result
    elif isinstance(root_dict, list):
        return list(map(lambda i: replace_dict_key_recursively(i, key_lookup), root_dict))
    else:
        return root_dict

if __name__ == '__main__':
    main()
