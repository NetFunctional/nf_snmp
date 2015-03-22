var snmpModule = require('netfunctional.snmp');
Ti.API.info("module is => " + snmpModule);

var snmp = snmpModule.createSNMPClient({
	'hostname': '24.150.215.37',
	'community': 'titanium',
	'mib': 's',
	'time_out': 100000 // 10 seconds
});

snmp.onload = function(e){
    Ti.API.info("ONLOAD RESULT: "+ e.message);
};
snmp.onerror = function(e){
	Ti.API.info("ONERROR RESULT: "+ e.message);
};

snmp.open();

// you can change the mib using: snmp.mib = 'system.sysDescr.0';
var result = snmp.walk();
if (result == null)
{
    Ti.API.info('Result is null');
}
else
{
    Ti.API.info("SNMP WALK results count: " + result.length);
    for (var i=0; i < result.length; i++) {
		Ti.API.info("SNMP WALK result: " + result[i]);
    };
}
snmp.close();