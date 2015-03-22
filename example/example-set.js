var snmpModule = require('netfunctional.snmp');
Ti.API.info("module is => " + snmpModule);

var snmp = snmpModule.createSNMPClient({
	'hostname': '24.150.215.37',
	'community': 'titanium-rw',
	'mib': 's',// 'system.sysUpTime.0',
	'time_out': 100000 // 10 seconds
});

snmp.onload = function(e){
    Ti.API.info("ONLOAD RESULT: "+ e.message);
};
snmp.onerror = function(e){
	Ti.API.info("ONERROR RESULT: "+ e.message);
};

Ti.API.info("open");
snmp.open();

var result = snmp.write("SNMPv2-MIB::sysContact.0", 's', "testing");
Ti.API.info('Result: ' + result);

snmp.close();