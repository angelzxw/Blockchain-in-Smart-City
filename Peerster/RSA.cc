#include "RSA.hh"


RSA::RSA()
{
	srand (time(NULL));

	p = getRandPrime();

	while(true)
	{
		q = getRandPrime();

		if(q != p)
		{
			break;
		}
	}

	n = p * q;
    t = (p - 1) * (q - 1);
    ce();
}

QString RSA::sha1(QString s)
{
	if(!QCA::isSupported("sha1"))
	{
		qDebug() << "SHA1 not supported!\n";
		return QString();
	}
    else
    {
    	QCA::SecureArray secByteArray = s.toUtf8();
        return QString(QCA::Hash("sha1").hash(secByteArray).toByteArray().toHex());
    }
}

int RSA::prime(long int pr)
{
    int i;
    long int j = sqrt(pr);
    for (i = 2; i <= j; i++)
    {
        if (pr % i == 0)
            return 0;
    }
    return 1;
}

int RSA::getRandPrime()
{
	while(true)
	{
		int r = rand() % 100;

		if(prime(r * 10 + 1) == 1)
		{
			return r * 10 + 1;
		}
	}
}

void RSA::ce()
{
	int k;
	long int flag;
	long int etemp[100];
	long int dtemp[100];

    k = 0;
    for (long int i = 2; i < t; i++)
    {
        if (t % i == 0)
            continue;
        flag = prime(i);
        if (flag == 1 && i != p && i != q)
        {
            etemp[k] = i;
            flag = cd(etemp[k]);
            if (flag > 0)
            {
                dtemp[k] = flag;
                k++;
            }
            if (k == 10)
                break;
        }
    }
    int r = rand()%11;
    e = etemp[r];
    d = dtemp[r];
    // 	long int flag;
    // for (long int i = 2; i < t; i++)
    // {
    //     if (t % i == 0)
    //         continue;
    //     flag = prime(i);
    //     if (flag == 1 && i != p && i != q)
    //     {
    //         e = i;
    //         flag = cd(e);
    //         if (flag > 0)
    //         {
    //             d = flag;
    //             break;
    //         }
    //     }
    // }
}

long int RSA::cd(long int x)
{
    long int k = 1;
    while (1)
    {
        k = k + t;
        if (k % x == 0)
            return (k / x);
    }
}
QVariantList RSA::encrypt(QString s, long int key)
{
	if(key == 0)
	{
		key = e;
	}
    long int pt, ct, k, len;
    long int i = 0;
    len = s.size();
    QVariantList en;
    while (i != len)
    {
        pt = s.at(i).unicode();

        k = 1;
        for (long int j = 0; j < key; j++)
        {
            k = k * pt;
            k = k % n;
        }
        ct = k;
        en.append((quint32)ct);
        i++;
    }

    //cout << "\nTHE ENCRYPTED MESSAGE IS\n";
    //qDebug() << "en: " << en;
    return en;
}
QString RSA::decrypt(QVariantList en, long int key, long int n)
{
	if(key == 0)
	{
		key = d;
	}
	if(n == 0)
	{
		n = this->n;
	}

    long int pt, ct, k;
    long int i = 0;
    long int len = en.size();
    QString msg;

    while (i < len)
    {
        ct = en.at(i).toUInt();
 
        k = 1;

        for (long int j = 0; j < key; j++)
        {
            k = k * ct;
            k = k % n;
        }

        pt = k;

        msg.append(QChar((uint)pt));
        i++;
    }

    //cout << "\nTHE DECRYPTED MESSAGE IS\n";
    //qDebug() << "msg: " <<msg;
    return msg;
}

long int RSA::getPubKey()
{
	return e;
}
long int RSA::getN()
{
	return n;
}

QVariantList RSA::signMessage(QString msg)
{
	return encrypt(sha1(msg), d);
}

bool RSA::verifySignature(QString msg, QVariantList encHash, long int pubkey, long int pubn)
{
	//qDebug() << "verifySignature: " <<sha1(msg) <<decrypt(encHash, pubkey, pubn);
	return sha1(msg) == decrypt(encHash, pubkey, pubn);
}

// int main()
// {
//     cout << "\nENTER FIRST PRIME NUMBER\n";
//     cin >> p;
//     flag = prime(p);
//     if (flag == 0)
//     {
//         cout << "\nWRONG INPUT\n";
//         exit(1);
//     }
//     cout << "\nENTER ANOTHER PRIME NUMBER\n";
//     cin >> q;
//     flag = prime(q);
//     if (flag == 0 || p == q)
//     {
//         cout << "\nWRONG INPUT\n";
//         exit(1);
//     }
//     cout << "\nENTER MESSAGE\n";
//     fflush(stdin);
//     cin >> msg;
//     for (i = 0; msg[i] != NULL; i++)
//         m[i] = msg[i];
//     n = p * q;
//     t = (p - 1) * (q - 1);
//     ce();
//     cout << "\nPOSSIBLE VALUES OF e AND d ARE\n";
//     for (i = 0; i < j - 1; i++)
//         cout << e[i] << "\t" << d[i] << "\n";
//     encrypt();
//     decrypt();
//     return 0;
// }
