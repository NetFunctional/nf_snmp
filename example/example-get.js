var snmpModule = require('netfunctional.snmp');
Ti.API.info("module is => " + snmpModule);

var snmp = snmpModule.createSNMPClient({
	'hostname': '24.150.215.37',
	'community': 'titanium',
	'mib': 's',// 'system.sysUpTime.0',
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
var result = snmp.fetch();
if (result == null)
{
	Ti.API.info('Result is null');
}
else
{
	Ti.API.info("SNMP GET result: " + result);
}
snmp.close();