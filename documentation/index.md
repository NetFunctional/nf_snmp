# netfunctional_snmp Module

## Description

The module allows you to communicate with SNMP enabled hosts, and fetch data
from them. Normally this is used for network management purposes - collecting
statistics, checking if an interface is Up/Down. Currently this module
implements the SNMP GET and WALK functionality.

## Accessing the netfunctional_snmp Module

To access this module from JavaScript, you would do the following:

    
    var snmpModule = require("netfunctional.snmp");
    

The snmpModule variable is a reference to the Module object.

## Reference

### `**netfunctional.snmp.createSNMPClient()**`

#### _Parameters:_

**`hostname`**: The hostname (DNS or IP address) of the SNMP enabled host

**`community`**: The SNMP community to use to talk to the SNMP host

**`mib`**: The ASN.1 MIB entry, in either numeric (ie: 1.3.6.1.#.#.#.#) or english (ie: system.sysDescr.0) to fetch from the SNMP host

**`time_out`**: Duration (in milliseconds) to wait for a response

### _Example _

    
    
    	var snmp = snmpModule.createSNMPClient({
    		'hostname': '127.0.0.1',  // Insert the IP address of an SNMP enabled host here.
    		'community': 'public',
    		'mib': 'system.sysUpTime.0',
    		'time_out': 2000 // 2 seconds
    	});
    	
    	

### `**netfunctional.snmp.fetch()**`

#### _Returns:_

**`string`**

Returns the data.

    
    
    	var result = snmp.fetch();
    	alert(result);
    	
    

### `**netfunctional.snmp.walk()**`

#### _Returns:_

**`array`**

Returns the results of a WALK request as an array.

    
    
    	var result = snmp.walk();
    	alert(result);
    	
    

### `**netfunctional.snmp.write()**`

#### _Parameters:_

**`oid`**: The ASN.1 MIB entry, in either numeric (ie: 1.3.6.1.#.#.#.#) or english (ie: system.sysDescr.0) to have the value changed

**`type`**: The type of the value to be set, as a single character. Must be one of the following values: i (INTEGER), u (unsigned INTEGER), t (TIMETICKS), a (IPADDRESS), o (OBJID), s (STRING), x (HEX STRING), d (DECIMAL STRING), b (BITS)

**`value`**: The new value for the oid

#### _Returns:_

**`bool`**

Changes the value of the oid with the provided one.

    
    
    	var result = snmp.write("system.sysContact.0", 's', "New Contact");
    	alert(result);
    	
    

## Usage

    
    
    	
    	var snmpModule = require("netfunctional.snmp");
    	
    	var snmphost = '127.0.0.1';
    	var snmp = snmpModule.createSNMPClient({
    		'hostname': snmphost,
    		'community': 'public',
    		'mib': 'system.sysLocation.0',
    		'time_out': 2000 // 2 seconds
    	});
    
    	// fetch()
    	var result = snmp.fetch();
    	alert(snmphost + ' is located at ' + result);
    
    	// walk()
    	var result = snmp.walk();
    	for (var i=0; i < result.length; i++) {
    		Ti.API.info("SNMP WALK result: " + result[i]);
    	};
    	
    	// write()
    	var result = snmp.write("system.sysContact.0", 's', "New Contact");
    	Ti.API.info('Result: ' + result);
    	
    	snmp.close();
    	

## Author

NetFunctional Inc. apps@netfunctional.ca

## License

See LICENSE file included in this distribution.

